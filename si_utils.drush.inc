<?php

/**
 * @file
 *
 * Functions used by all of the smithsonian drush commands.
 */

/* Constants */
define('SI_DRUSH_COMMAND_DIR', dirname(__FILE__));
define('SI_DEFAULT_POLICY', SI_DRUSH_COMMAND_DIR . '/data/datastreams/POLICY.xml');
define('SI_DEFAULT_COLLECTION_POLICY', SI_DRUSH_COMMAND_DIR . '/data/datastreams/COLLECTION_POLICY.xml');

/**
 * Creates a object with the given attributes.
 *
 * @param string $thumbnail
 *   The path to the file to be used as a thumbnail for this object.
 */
function si_create_object(array &$properties, $thumbnail) {
  $object = si_create_empty_object($properties['label'], $properties['id']);
  $properties['pid'] = $object->pid;
  si_update_dc_description($object, $properties['description']);
  si_add_thumbnail($object, $thumbnail);
  si_add_policy($object);
  return $object;
}

/**
 * Creates a empty Fedora Object.
 *
 * @param string $label
 *   The label for the object.
 * @return Fedora_Item
 *   The newly created Fedora_Item.
 */
function si_create_empty_object($label, $id) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $pid = Fedora_Item::get_next_PID_in_namespace('si');
  drush_print("Creating object: {$label} ($pid/$id)");
  $object = Fedora_Item::ingest_new_item($pid, 'A', $label);
  return $object;
}

/**
 * Updates and objects DC datastreams dc:description element with the given description.
 *
 * @param Fedora_Item $object
 *   The object to modify.
 * @param string $description
 *   The description to add to the DC.
 */
function si_update_dc_description(Fedora_Item $object, $description) {
  module_load_include('inc', 'fedora_repository', 'api/dublin_core');
  $dc = new Dublin_Core($object);
  if (isset($description)) {
    $dc->set_element('dc:description', $description);
  }
  else {
    drush_print("Error adding description to dc, description not set.");
  }
  $dc->save();
}

/**
 * Adds a thumbnail datastream to the object.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 * @param string $thumbnail
 *   The name of the thumbnail in the images directory.
 */
function si_add_thumbnail(Fedora_Item $object, $thumbnail) {
  $directory = dirname(__FILE__);
  si_add_datastream_from_file($object, 'TN', "$directory/data/images/$thumbnail", 'image/png');
}

/**
 * Adds a POLICY datastream to the object.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 */
function si_add_policy(Fedora_Item $object) {
  $directory = dirname(__FILE__);
  si_add_datastream_from_file($object, 'POLICY', "$directory/data/datastreams/POLICY.xml", 'text/xml');
}

/**
 * Adds a datastream based on the given attributes, where URL points to a file to be added.
 *
 * @param Fedora_Item $object
 *  The object to add the datastream to.
 * @param string $dsid
 *  The datastream id
 * @param string $url
 *  The url to the file to add
 * @param string $mimetype
 *  The mime type of the datastream
 * @param string $label
 *  The label of the datastream
 * @param string $control_group
 *  The control group for the datastream
 */
function si_add_datastream_from_file(Fedora_Item $object, $dsid, $file, $mimetype, $label = NULL, $control_group = 'M') {
  $label = isset($label) ? $label : $dsid;
  if ($object->add_datastream_from_file($file, $dsid, $label, $mimetype, $control_group, "Added $dsid Stream")) {
    drush_print("Added datastream $dsid to {$object->pid}");
  }
  else {
    drush_print("Failed adding $dsid to {$object->pid} using file $url");
  }
}

/**
 * Adds a datastream based on the given attributes, where the content of the datastream is given in $string.
 *
 * @param Fedora_Item $object
 *  The object to add the datastream to.
 * @param string $dsid
 *  The datastream id
 * @param string $string
 *  The content of the datastream
 * @param string $mimetype
 *  The mime type of the datastream
 * @param string $label
 *  The label of the datastream
 * @param string $control_group
 *  The control group for the datastream
 */
function si_add_datastream_from_string(Fedora_Item $object, $dsid, $string, $mimetype = 'text/xml', $label = '', $control_group = 'M') {
  $label = isset($label) ? $label : $dsid;
  if ($object->add_datastream_from_string($string, $dsid, $label, $mimetype, $control_group, "Added $dsid Stream")) {
    drush_print("Added datastream $dsid to {$object->pid}");
  }
  else {
    drush_print("Failed adding $dsid to {$object->pid} using file $url");
  }
}

/**
 * Adds a literal value to the RELS-EXT datastream so that it can be used in the Resource Index.
 *
 * @param Fedora_Item $object
 *   The object to add the relationship to.
 * @param string $predicate
 *   The relationship predicate.
 * @param string $literal
 *   The value of the relationship
 * @param string $uri
 *   The namespace in which to add the relationship.
 */
function si_add_external_relation_literal(Fedora_Item $object, $predicate, $literal, $uri) {
  $ds_list = $object->get_datastreams_list_as_array();
  if (empty($ds_list['RELS-EXT'])) {
    $empty_rels_ext = "<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>
       <rdf:Description rdf:about='info:fedora/{$object->pid}'>
       </rdf:Description>
    </rdf:RDF>";
    $object->add_datastream_from_string($empty_rels_ext, 'RELS-EXT', 'Fedora object-to-object relationship metadata', 'text/xml', 'X');
  }
  $rels_ext = $object->get_datastream_dissemination('RELS-EXT');
  $xml = new SimpleXMLElement($rels_ext);
  $description = array_shift($xml->xpath('//*[local-name() = "Description" or local-name() = "description"]'));
  $relationship = $description->addChild($predicate, $literal, $uri);
  $object->modify_datastream_by_value($xml->asXML(), 'RELS-EXT', "Fedora Object-to-Object Relationship Metadata", 'text/xml');
}

/**
 * Adds a COLLECTION_POLICY datastream to the object. If no policy is given a default one will be used.
 *
 * Newly added COLLECTION_POLICY datastreams are managed by default.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the COLLECTION_POLICY to.
 * @param string $policy
 *   The path to the collection policy file to add.
 */
function si_add_collection_policy(Fedora_Item $object, $policy = NULL) {
  $policy = isset($policy) ? $policy : SI_DEFAULT_COLLECTION_POLICY;
  si_add_datastream_from_file($object, 'COLLECTION_POLICY', $policy, 'text/xml');
}

/**
 * Updates the COLLECTION_POLICY datastream to the object. If no policy is given a default one will be used.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 * @param string $policy
 *   The path to the collection policy file to add.
 */
function si_update_collection_policy(Fedora_Item $object, $policy = NULL) {
  $policy = isset($policy) ? $policy : SI_DEFAULT_COLLECTION_POLICY;
  $datastream_info = $object->get_datastream_info("COLLECTION_POLICY");
  $inline = (string) $datastream_info->datastream->controlGroup == 'X';
  $policy = $inline ? file_get_contents($policy) : $policy;
  $function = $inline ? 'modify_datastream_by_value' : 'modify_datastream_by_reference';
  $success = $object->$function($policy, 'COLLECTION_POLICY', 'COLLECTION_POLICY', 'text/xml',  FALSE,  'Updated via drush');
  $message = $success ? "Successfully updated COLLECTION_POLICY - ({$object->pid})" : "Error updating COLLECTION_POLICY - ({$object->pid})";
  drush_print($message);
}

/**
 * Adds/Updates the collection policy of the given object.
 *
 * Newly added COLLECTION_POLICY datastreams are managed by default.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 * @param string $policy
 *   The path to the collection policy file to add.
 */
function si_set_collection_policy(Fedora_Item $object, $policy = NULL) {
  $has_policy = isset($object->datastreams["COLLECTION_POLICY"]);
  $has_policy ? si_update_collection_policy($object, $policy) : si_add_collection_policy($object, $policy);
}

/**
 * Transforms the given input with the given tranformation.
 *
 * @param string $file
 *   The path to the file to be tranformed.
 * @param string $tranform.
 *   The path to the xsl transform to apply.
 *
 * @returns string
 *   The transformed $file.
 */
function si_transform_xml_file($file, $transform) {
  return si_transform_xml(file_get_contents($file), $transform);
}

/**
 * Transforms the given input with the given tranformation.
 *
 * @param string $input
 *   The xml string to be tranformed.
 * @param string $tranformn.
 *   The path to the xsl transform to apply.
 *
 * @returns string
 *   The transformed $input.
 */
function si_transform_xml($input, $transform) {
  drush_print("Transforming $input with $transform");
  $xslt = new XSLTProcessor;
  $xslt->importStyleSheet(DOMDocument::load($transform));
  return $xslt->transformToXML(DOMDocument::loadXML($input));
}

/**
 * Split the xml file based on a root tag.
 *
 * @param string $input
 *   The xml string to be split.
 * @param string $root_element.
 *   The case-sensitive name of the root element in the transformed records.
 *
 * @returns array
 *   Each record as a string.
 */
function si_split_xml_file($input, $root_element) {
  $closing_tag = "</$root_element>";
  $records = explode($closing_tag, $input);
  $records = array_map('trim', $records);
  $records = array_filter($records);
  foreach($records as &$record) {
    $record .= $closing_tag;
  }
  $count = count($records);
  drush_print("Transformed $count records in $input");
  return $records;
}

/**
 * Gets the string as a simple xml object, with 'ns' as the default namespace.
 *
 * @param string $input
 *   The xml string to get as a simpleXMLElement;
 */
function si_get_simple_xml($input) {
  $xml = simplexml_load_string($input);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  $xml->registerXPathNamespace('xlink', $namespaces['xlink']);
  return $xml;
}

/**
 * Gets a single value from the node specified by the given $xpath
 *
 * @return string
 *   The requested value if found, otherwise an empty string.
 */
function si_get_value_from_xml(SimpleXMLElement &$xml, $xpath) {
  $results = $xml->xpath($xpath);
  return is_array($results) ? htmlspecialchars((string) array_shift($xml->xpath($xpath)), ENT_NOQUOTES, 'UTF-8') : '';
}

/**
 * Gets a values from the nodes specified by the given $xpath
 *
 * @return array
 *   The requested values if found, otherwise an empty array.
 */
function si_get_values_from_xml(SimpleXMLElement &$xml, $xpath) {
  $results = $xml->xpath($xpath);
  return is_array($results) ? array_map(function($o) { return htmlspecialchars((string) $o, ENT_NOQUOTES, 'UTF-8'); }, $results) : array();
}

/**
 * Normalizes the label for the fedora object.
 */
function si_normalize_object_label($label) {
  // @TODO do the full regex check
  return (strlen($label) > 255) ? substr($label, 0, 254) : $label;
}

/**
 * Runs an RISearch query.
 *
 * @return string
 *  The results of the RISearch query if successful, FALSE otherwise.
 */
function si_run_query($query) {
  module_load_include('inc', 'fedora_repository', 'CollectionClass');
  $cc = new CollectionClass();
  $results = $cc->getRelatedItems(NULL, $query,1000000);
  if (isset($results)) {//still doesn't tell us if these are valid results
    return $results;
  }
  else {
    drush_print('Error get related items, relationships cannot be updated');
  }
  return FALSE;
}

/**
 * Converts the given sparql results to an array.
 *
 * @param string $xml
 *   The risearch results in sparql format.
 *
 * @return array
 *   The sparql results as an array.
 */
function si_sparql_results_as_array($xml) {
  $content = new SimpleXMLElement($xml);
  $ret = array();
  foreach ($content->results->result as $result) {
    $ret[] = substr($result->object->attributes()->uri, 12); // Remove 'info:fedora/'.
  }
  return $ret;
}