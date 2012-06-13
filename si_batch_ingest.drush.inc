<?php

/**
 * Batch ingest script for Smithsonian Fieldbook data.
 */
define('SMITHSONIAN_ONTOLOGY_URI', 'info:islandora/islandora-system:def/smithsonian#');
define('EACP_ID_PREDICATE', 'eacpId');
define('EACO_ID_PREDICATE', 'eacoId');
define('EACE_ID_PREDICATE', 'eaceId');
define('NCD_ID_PREDICATE', 'ncdId');
define('MODS_ID_PREDICATE', 'modsId');
define('ALT_ID_PREDICATE', 'altId');
define('TYPE_PERSON', 'EACP'); // Smaller string takes up less space.
define('TYPE_ORGANIZATION', 'EACO');
define('TYPE_EXPEDITION', 'EACE');
define('TYPE_COLLECTION', 'NCD');
define('TYPE_FIELDBOOK', 'MODS');

/**
 * Defines the drush command for ingesting Fieldbook data.
 */
function si_batch_ingest_drush_command() {
  return array(
    'si_batch_ingest_fieldbooks' => array(
      'callback' => 'drush_si_batch_ingest_fieldbooks',
      'description' => "Ingests all fieldbook related data, make sure the proper data is in the drush directory.",
      'examples' => array(
        'drush ingest-fieldbooks',
      ),
      'aliases' => array('ingest-fieldbooks'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // We can pass in users id on the command line using drush -u.
    )
  );
}

/**
 * Generate/Ingest all field book related data. Make relevant relationships between the ingested data.
 */
function drush_si_batch_ingest_fieldbooks() {
  $url = variable_get('fedora_base_url', 'http://localhost:8080/fedora');
  drush_print("Batch Ingesting Fieldbooks into Fedora repository: $url");
  $objects = si_batch_ingest_all();
  si_batch_update_all($objects);
  drush_print("The batch ingest was successful");
}

/**
 * Validate the arguments provided to this drush command.
 */
function drush_si_batch_ingest_fieldbooks_validate() {
  global $user;
  if($user->uid != 1) {
    return drush_set_error('INVALID_ARGUMENT', dt('Must be the admin user to run this command, specify "-u 1" as an argument.'));
  }
  $valid_uris = array('http://drupal.local/nbanks', 'http://si-islandora.si.edu', 'http://si-islandora.si.edu/test'); // @TODO remove 'http://drupal.local/nbanks'
  $cli = drush_get_context('cli');
  if(!isset($cli['uri'])) {
    return drush_set_error('INVALID_ARGUMENT', dt('The argument --uri is required. Please use either "http://si-islandora.si.edu" or "http://si-islandora.si.edu/test"'));
  }
  else if(array_search($cli['uri'], $valid_uris) === FALSE) {
    return drush_set_error('INVALID_ARGUMENT', dt('The --uri given is not valid. Please use either "http://si-islandora.si.edu" or "http://si-islandora.si.edu/test"'));
  }
}

/**
 * Batch ingest all the availible objects.
 *
 * @return array
 *   An array of all the objects created, grouped by the types defined at the top of this file.
 */
function si_batch_ingest_all() {
  static $ingest_functions = array(
    TYPE_PERSON => 'si_ingest_persons',
    TYPE_ORGANIZATION => 'si_ingest_organizations',
    TYPE_EXPEDITION => 'si_ingest_expeditions',
    TYPE_COLLECTION => 'si_ingest_collections',
    TYPE_FIELDBOOK => 'si_ingest_fieldbooks'
  );
  $objects = array();
  foreach($ingest_functions as $key => $function) {
    $objects[$key] = $function();
  }
  $count = array_reduce($objects, function($result, $object_group) { return $result + count($object_group); }, 0);
  drush_print("Ingested $count Objects");
  return $objects;
}

/**
 * Batch update all the ingested objects.
 */
function si_batch_update_all(array $objects) {
  foreach($objects as $key => $grouped_objects) {
    foreach($grouped_objects as $object) {
      si_update_object($object, $key, $objects);
    }
  }
}

/**
 * Ingest the given records using the given function.
 */
function si_ingest_records(array $records, string $function) {
  $objects = array();
  foreach($records as $record) {
    $objects[] = $function($record);
  }
  return $objects;
}

/**
 * Generates/Ingests EAC-CPF records for each row in the FileMaker Pro dump of Person Records.
 *
 * @return array
 *   A description of the ingested object, including any relevent information for updating its relations.
 */
function si_ingest_persons() {
  $records = si_transform('person.xml', 'EAC_person.xsl', 'eac-cpf');
  return si_ingest_records($records, 'si_ingest_person');
}

/**
 * Ingests a single person record.
 *
 * @param string $record
 *   The EAC-CPF Person record to ingest.
 *
 * @return array
 *   A description of the ingested object.
 */
function si_ingest_person(string $record) {
  $data = si_eaccpf_get_info($record);
  $object = si_create_object($data, 'person.png');
  si_add_datastream_from_string($object, 'EAC-CPF', $record);
  $object->add_relationship('isMemberOfCollection', 'si:people');
  $object->add_relationship('hasModel', 'si:peopleCModel', FEDORA_MODEL_URI);
  si_add_relation_literal($object, EACP_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
}

/**
 * Generates/Ingests EAC-CPF records for each row in the FileMaker Pro dump of the Organization Records.
 *
 * @return array
 *   A description of the ingested object, including any relevent information for updating its relations.
 */
function si_ingest_organizations() {
  $records = si_transform('organization.xml', 'EAC_organization.xsl', 'eac-cpf');
  return si_ingest_records($records, 'si_ingest_organization');
}

/**
 * Ingests a single organization record.
 *
 * @param string $record
 *   The EAC-CPF Organization record to ingest.
 *
 * @return array
 *   A description of the ingested object.
 */
function si_ingest_organization(string $record) {
  $data = si_eaccpf_get_info($record);
  $object = si_create_object($data, 'organization.png');
  si_add_datastream_from_string($object, 'EAC-CPF', $record);
  $object->add_relationship('isMemberOfCollection', 'si:organizations');
  $object->add_relationship('hasModel', 'si:organizationCModel', FEDORA_MODEL_URI);
  si_add_relation_literal($object, EACO_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
}

/**
 * Generates/Ingests EAC-CPF records for each row in the FileMaker Pro dump of the Expeditions Records.
 *
 * @return array
 *   A description of the ingested object, including any relevent information for updating its relations.
 */
function si_ingest_expeditions() {
  $records = si_transform('expedition.xml', 'EAC_expedition.xsl', 'eac-cpf');
  return si_ingest_records($records, 'si_ingest_organization');
}

/**
 * Ingests a single expedition record.
 *
 * @param string $record
 *   The EAC-CPF Expedition record to ingest.
 *
 * @return array
 *   A description of the ingested object.
 */
function si_ingest_expedition(string $record) {
  $data = si_eaccpf_get_info($record);
  $object = si_create_object($data, 'organization.png');
  si_add_datastream_from_string($object, 'EAC-CPF', $record);
  $object->add_relationship('isMemberOfCollection', 'si:expeditions');
  $object->add_relationship('hasModel', 'si:expeditionCModel', FEDORA_MODEL_URI);
  si_add_relation_literal($object, EACE_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
}

/**
 * Generates/Ingests NCD records for each row in the FileMaker Pro dump of the Collection Records.
 *
 * @return array
 *   A description of the ingested object, including any relevent information for updating its relations.
 */
function si_ingest_collections() {
  $records = si_transform('collection.xml', 'NCD_collection.xsl', 'RecordSet');
  return si_ingest_records($records, 'si_ingest_collection');
}

/**
 * Ingests a single collection record.
 *
 * @param string $record
 *   The NCD collection record to ingest.
 *
 * @return array
 *   A description of the ingested object.
 */
function si_ingest_collection(string $record) {
  $data = si_ncd_get_info($record);
  $object = si_create_object($data, 'collection.png');
  si_add_datastream_from_string($object, 'NCD', $record);
  $object->add_relationship('isMemberOfCollection', 'si:collections');
  $object->add_relationship('hasModel', 'si:ncdCollectionCModel', FEDORA_MODEL_URI);
  si_add_relation_literal($object, NCD_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
}

/**
 * Generates/Ingests MODS records for each row in the FileMaker Pro dump of the Fieldbook Records.
 *
 * @return array
 *   A description of the ingested object, including any relevent information for updating its relations.
 */
function si_ingest_fieldbooks() {
  $records = si_transform('fieldbook.xml', 'MODS_fieldbook.xsl', 'mods');
  return si_ingest_records($records, 'si_ingest_fieldbook');
}

/**
 * Ingests a single fieldbook record.
 *
 * @param string $record
 *   The MODS fedora record to ingest.
 *
 * @return array
 *   A description of the ingested object.
 */
function si_ingest_fieldbook(string $record) {
  $data = si_mods_get_info($record);
  $object = si_create_object($data, 'fieldbook.png');
  si_add_datastream_from_string($object, 'MODS', $record);
  $object->add_relationship('hasModel', 'si:fieldbookCModel', FEDORA_MODEL_URI);
  $object->add_relationship('isMemberOfCollection', 'si:fieldbooks');
  si_add_relation_literal($object, MODS_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  if(!empty($data['alt_id'])) {
    si_add_relation_literal($object, ALT_ID_PREDICATE, $data['alt_id'], SMITHSONIAN_ONTOLOGY_URI);
  }
  $object->forget(); // Prevent Memory Leaks
  return $data;
}

/**
 * Transforms the given input with the given tranformation.
 *
 * The given files are expected to live in the data/xsl directories respectively.
 *
 * @param string $input
 *   The file to be tranformed.
 * @param string $tranformn to be applied.
 *   The xsl transform to apply
 * @param string $root_element
 *   The case-sensitive name of the root element in the transformed records.
 *
 * @returns array
 *   Each record as a string.
 */
function si_transform($input, $transform, $root_element) {
  drush_print("Transforming $input with $transform");
  $directory = dirname(__FILE__);
  $xslt = new XSLTProcessor;
  $xslt->importStyleSheet(DOMDocument::load("$directory/xsl/$transform"));
  $records = $xslt->transformToXML(DOMDocument::loadXML(file_get_contents("$directory/data/$input")));
  $closing_tag = "</$root_element>";
  $records = explode($closing_tag, $records);
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
 * Creates a object with the given attributes.
 */
function si_create_object(array &$properties, string $thumbnail) {
  $object = si_create_empty_object($properties['label']);
  $properties['pid'] = $object->pid;
  si_update_dc_description($object, $properties['description']);
  si_add_thumbnail($object, $thumbnail);
  return $object;
}

/**
 * Creates a empty Fedora Object.
 *
 * @param string $label
 *   The label for the object.
 * @return FedoraItem
 *   The newly created FedoraItem.
 */
function si_create_empty_object(string $label) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $pid = Fedora_Item::get_next_PID_in_namespace('si');
  drush_print("Creating object: {$label} ($pid)");
  $object = Fedora_Item::ingest_new_item($pid, 'A', $label);
  return $object;
}

/**
 * Updates and objects DC datastreams dc:description element with the given description.
 *
 * @param FedoraItem $object
 *   The object to modify.
 * @param string $description
 *   The description to add to the DC.
 */
function si_update_dc_description(FedoraItem $object, $description) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  module_load_include('inc', 'fedora_repository', 'api/dublin_core');
  $dc = new Dublin_Core($object);
  if (isset($description)) {
    $dc->set_element('dc:description', $description);
  }
  else {
    drush_print("error adding description to dc, description not set.");
  }
  $dc->save();
}

/**
 * Adds a thumbnail datastream to the object.
 *
 * @param FedoraItem $object
 *   The fedora object to add the thumbnail to.
 * @param string $thumbnail
 *   The name of the thumbnail in the images directory.
 */
function si_add_thumbnail(FedoraItem $object, string $thumbnail) {
  $directory = dirname(__FILE__);
  si_add_datastream_from_file($object, 'TN', "$directory/images/$thumbnail", 'image/png');
}

/**
 * Adds a datastream based on the given attributes, where URL points to a file to be added.
 *
 * @param FedoraItem $object
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
function si_add_datastream_from_file(FedoraItem $object, string $dsid, string $file, string $mimetype, string $label = NULL, $control_group = 'M') {
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
 * @param FedoraItem $object
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
function si_add_datastream_from_string(FedoraItem $object, string $dsid, string $string, $mimetype = 'text/xml', $label = '', $control_group = 'M') {
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
function si_add_relation_literal(FedoraItem $object, string $predicate, string $literal, string $uri) {
  $ds_list = $object->get_datastreams_list_as_array();
  if (empty($ds_list['RELS-EXT'])) {
    $empty_rels_ext =
	"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>
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
 * Gets useful information from the given eac-cpf record.
 *
 * @param string $record
 *   The EAC-CPF record to query.
 *
 * @return array
 *   An array of info.
 */
function si_eaccpf_get_info(string $record) {
  $xml = simplexml_load_string($record);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  $id = (string) array_shift($xml->xpath('//ns:recordId'));
  $label = (string) array_shift($xml->xpath('//ns:identity/ns:nameEntry[@localType="primary"]/ns:part'));
  $description = (string) array_shift($xml->xpath('//ns:biogHist/ns:p'));
  $label = si_normalize_object_label($label);
  return array(
    'id' => $id,
    'label' => si_normalize_object_label($label),
    'description' => $description
  );
}

/**
 * Gets useful information form the given NCD record.
 *
 * @param string $record
 *   The NCD record to query.
 *
 * @return array
 *   An array of info.
 */
function si_ncd_get_info(string $record) {
  $xml = simplexml_load_string($record);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  $id = (string) array_shift($xml->xpath('//ns:CollectionId'));
  $alt_id = (string) array_shift($xml->xpath('//ns:AlternativeIds/ns:Identifier[1]'));
  $label = (string) array_shift($xml->xpath('//ns:dc_title'));
  $description = (string) array_shift($xml->xpath('//ns:dc_description'));
  $label = si_normalize_object_label($label);
  return array(
    'id' => $id,
    'alt_id' => $alt_id,
    'label' => si_normalize_object_label($label),
    'description' => $description
  );
}

/**
 * Gets useful information form the given MODS record.
 *
 * @param string $record
 *   The EAC-CPF record to query.
 *
 * @return array
 *   An array of info.
 */
function si_mods_get_info(string $record) {
  $xml = simplexml_load_string($record);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  $id = (string) array_shift($xml->xpath('/ns:mods/@ID'));
  $alt_id = (string) array_shift($xml->xpath('/ns:mods/ns:identifier[@type="accessionNumber"]'));
  $label = (string) array_shift($xml->xpath('//ns:titleInfo/ns:title'));
  $description = (string) array_shift($xml->xpath('//ns:titleInfo/ns:abstract'));
  $to_string = function($o) { return (string) $o; };
  $related_names = array_map($to_string, $xml->xpath('//ns:name/@valueURI'));
  $related_items = array_map($to_string, $xml->xpath('//ns:relatedItem/ns:titleInfo/@valueURI'));
  return array(
    'id' => $id,
    'alt_id' => $alt_id,
    'label' => si_normalize_object_label($label),
    'description' => $description,
    'related_names' => $related_names,
    'related_items' => $related_items,
  );
}

/**
 * Normalizes the label for the fedora object.
 */
function si_normalize_object_label(string $label) {
  // @TODO do the full regex check
  return (strlen($label) > 255) ? substr($label, 0, 254) : $label;
}

/**
 * Updates the given object using data stored in other objects.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param string $type
 *   The TYPE of the object properties as defined at the top of this file.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_object(array $object, string $type, array $objects) {
  static $update_functions = array(
    TYPE_PERSON => 'si_update_person',
    TYPE_ORGANIZATION => 'si_update_organization',
    TYPE_EXPEDITION => 'si_update_expedition',
    TYPE_COLLECTION => 'si_update_collection',
    TYPE_FIELDBOOK => 'si_update_fieldbook'
  );
  $function = $update_functions[$type];
  drush_print("Updating object: {$object['label']} ({$object['pid']})");
  $function($object, $objects);
}

/**
 * Update the Person Object identified by the properties in $object.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_person(array $object, array $objects) {

}

/**
 * Update the Organization Object identified by the properties in $object.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_organization(array $object, array $objects) {

}

/**
 * Update the Expedition Object identified by the properties in $object.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_expedition(array $object, array $objects) {

}

/**
 * Update the Collection Object identified by the properties in $object.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_collection(array $object, array $objects) {

}

/**
 * Update the Field Object identified by the properties in $object.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_fieldbook(array $object, array $objects) {
  module_load_include('module', 'fedora_repository', 'api/fedora_item');
  $item = new Fedora_Item($object['pid']);
  $mods = $item->get_datastream_dissemination('MODS');
  $xml = simplexml_load_string($mods);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  si_update_references_with_label($xml, $object['related_names'], '//ns:name[@valueURI="%id"]/ns:namePart', $objects);
  si_update_references_with_label($xml, $object['related_items'], '//ns:relatedItem/ns:titleInfo[@valueURI="%id"]/ns:title', $objects);
  drush_print($xml->asXML());
  $item->modify_datastream_by_value($xml->asXML(), 'MODS', 'MODS', 'text/xml');
}

/**
 * Updates the references found at $xpath.
 */
function si_update_references_with_label(SimpleXMLElement &$xml, array $ids, string $xpath, array $objects) {
  foreach($ids as $id) {
    if($object = si_find_object($id, $objects)) {
      $label = $object['label'];
      $query = str_replace('%id', $id, $xpath);
      drush_print($query);
      $elements = $xml->xpath($query);
      foreach($elements as $element) {
        $element->{0} = $label;
      }
    }
  }
}

/**
 * Searchs the objects for one that matches both the given $id and $type.
 *
 * @param string $id
 *   The id of the given object.
 * @param string $type
 *   The type of object, matches the TYPE declarations at the top of the file.
 * @param array $objects
 *   Properties that define all the objects.
 *
 * @return array
 *   The object properties we were looking for if found, otherwise FALSE.
 */
function si_find_object(string $id, array $objects) {
  $type = si_id_type($id);
  $objects = $objects[$type];
  $result = array_filter($objects, function($object) use($id) {
              return $object['id'] == $id;
            });
  return empty($result) ? FALSE : array_shift($result);
}

/**
 * Returns the type for the ID.
 */
function si_id_type(string $id) {
  $types = array(TYPE_PERSON, TYPE_ORGANIZATION, TYPE_EXPEDITION, TYPE_COLLECTION, TYPE_FIELDBOOK);
  foreach($types as $type) {
    if(strpos($id, $type) !== FALSE) {
      return $type;
    }
  }
}

/**
 *
 *
 * @param string $collection_pid
 * @param string $relationship
 * @param string $input_file
 * @param string $input_xsl
 * @param string $url_to_tn
 *
function si_doAction($collection_pid, $relationship, $input_file, $input_xsl, $url_to_tn, $type) {
  $xml_record = si_transform($input_file, $input_xsl);
  $type = (string) trim($type);
  $object = null;
  switch($type) {
    case 'collection': // NCD
      $xml_records = explode("</RecordSet>", $xml_record); // Split into seperate XML Documents
      foreach ($xml_records as $record) {
        $record = $record . '</RecordSet>';
        si_collection_create_object($collection_pid, $relationship, $record, $url_to_tn);
      }
      break;
    case 'item': // MODS
      $xml_records = explode("</mods>", $xml_record); // Split into XML documents
      $original_records = simplexml_load_file($input_file); // There is content in the original that does not map to mods but we need to manage relationships
      $i = 0;
      foreach ($xml_records as $record) {
        $record = $record . '</mods>';
        $xml = simplexml_load_string($record);
        $namespaces = $xml->getDocNamespaces();
        $xml->registerXPathNamespace('mods', $namespaces['']);
        $or = $original_records->ROW[$i++]; // This should give us the corresponding original record
        $or = $or->asXML();
        $or_xml = simplexml_load_string($or);
        $relationship_data = si_get_fmp_relationship_data($or_xml);
        $object = si_item_create_object($collection_pid, $relationship, $xml, $url_to_tn); // Create the Fedora object with MODS data.
        $object->add_relationship('hasModel', 'si:fieldbookCModel', FEDORA_MODEL_URI);
        $description = $xml->xpath('//mods:abstract');
        si_update_dc($object, $description[0]);
        si_updateAllRelationships($object, $relationship_data);
      }
      break;
    case 'person':
    case 'organization':
    case 'expedition': // EAC-CPF
      $xml_records = explode("</eac-cpf>", $xml_record);
      foreach ($xml_records as $record) {
        $record = $record . '</eac-cpf>';
        $xml = simplexml_load_string($record);
        $namespaces = $xml->getDocNamespaces();
        $xml->registerXPathNamespace('ns', $namespaces['']);
        ///default:eac-cpf/default:cpfDescription[1]/default:identity[1]/default:nameEntry[1]/default:part[1]
        $result = $xml->xpath('//ns:identity/ns:nameEntry[@localType="primary"]/ns:part');
        $label = (string) $result[0];
        $result = $xml->xpath('//ns:recordId');
        $eac_id = (string) $result[0]; //we want to add this to the triple store so we can query when ingesting other objects
        $object = si_create_object($collection_pid, $relationship, $xml, $url_to_tn);
        //default:eac-cpf/default:cpfDescription[1]/default:description[1]/default:biogHist[1]/default:p[1]
        $result = $xml->xpath('//ns:biogHist/ns:p');
        si_update_dc($object, (string) $result[0]);
        switch ($type) {
          case 'person':
            $object->add_relationship('hasEacCpfPerson', $eac_id, EAC_PERSON_ID_NS);
            $object->add_relationship('hasModel', 'si:peopleCModel', FEDORA_MODEL_URI);
            break;
          case 'organization':
            $object->add_relationship('hasModel', 'si:organizationCModel', FEDORA_MODEL_URI);
            $object->add_relationship('hasEacCpfOrganization', $eac_id, EAC_ORGANIZATION_ID_NS);
            break;
          case 'expedition':
            drush_print('adding relationship for expedition');
            $object->add_relationship('hasEacCpfExpedition', $eac_id, EAC_EXPEDITION_ID_NS);
            $object->add_relationship('hasModel', 'si:expeditionCModel', FEDORA_MODEL_URI);
            break;
        }
      }
      break;
  }
}*/



/**
 * reads the orginal xml before we tranform to mods
 * gets the fields that hold relationhsip info and returns
 * an array contain this information
 * @param string $xml
 * @return array
 */
function si_get_fmp_relationship_data($xml) {
  //$namespaces = $xml->getDocNamespaces();
  //$xml->registerXPathNamespace('ns', $namespaces['']);
  $data = array();
  ///default:FMPDSORESULT/default:ROW[1]/default:i_creator[1]/default:DATA[1]
  $creators = $xml->xpath('//i_creator/DATA');
  foreach ($creators as $creator) {
    if (!empty($creator)) {//there are some fields without data
      $data['creators'][] = (string) $creator;
      print('added Creator ' . (string) $creator);
    }
  }
  $expeditions = $xml->xpath('//i_expedition_name/DATA');
  foreach ($expeditions as $exp) {
    if (!empty($exp)) {//there are some fields without data
      $data['expedition'][] = (string) $exp;
    }
  }
  $collections = $xml->xpath('//i_collection');
  foreach ($collections as $collection) {
    if (!empty($collection)) {//there are some fields without data
      $data['collections'][] = (string) $collection;
    }
  }
  return $data;
}

/**
 * the item mods and fmp-xml records have all the relationships so we will update the rels-ext of the objects
 * here. we have stored the needed info from the other objects in their rels-ext so we can query them
 * @param fedora_item $object
 * @param array $relationship_data
 */
function si_updateAllRelationships($object, $relationship_data) {
  //run resource index queries to get the objects we are interested in
  //<info:islandora/islandora-system:def/eacPersonId#hasEacCpfPerson> <info:fedora/EACP11> typical query to get creator
  //this item relates directly to these objects
  foreach ($relationship_data['creators'] as $creator) {
    $query = 'select $object from <#ri> where $object <' . EAC_PERSON_ID_NS . 'hasEacCpfPerson> <info:fedora/' . $creator . '>';
    $results = si_run_query($query);
    $objects = si_sparql_results_as_array($results); //turn xml into array
    foreach ($objects as $creator) {
      $object->add_relationship('hasCreator', (string) $creator);
      $object->add_relationship('isMemberOfCollection', (string) $creator);
      drush_print('added hasCreator relationship to ' . $creator);
    }
  }
  foreach ($relationship_data['expedition'] as $expedition) {
    $query = 'select $object from <#ri> where $object <' . EAC_EXPEDITION_ID_NS . 'hasEacCpfExpedition> <info:fedora/' . $expedition . '>';
    $results = si_run_query($query);
    $objects = si_sparql_results_as_array($results); //turn xml into array
    foreach ($objects as $exp) {
      //$object->add_relationship('isPartOfExpedition', (string) $exp);
      $object->add_relationship('isMemberOfCollection', (string) $exp);
      drush_print('added isPartOfExpedition relationship to ' . $exp);
    }
  }
  foreach ($relationship_data['collections'] as $collection) {
    $query = 'select $object from <#ri> where $object <' . NCD_COLLECTION_ID_NS . 'hasNcdId> <info:fedora/' . $collection . '>';
    $results = si_run_query($query);
    $objects = si_sparql_results_as_array($results); //turn xml into array
    foreach ($objects as $collection) {
      $object->add_relationship('isMemberOfCollection', (string) $collection);
      drush_print('added isMemberOfCollection relationship to ' . $collection);
      //now do the indirect relationships we can relate a collection to an expedition using the info gathered from this object.
      if (!empty($relationship_data['expedition'])) {
        si_relate_collection_to_expeditions($collection, $relationship_data['expedition']);
      }
    }
  }
}

/**
 *
 * @param string $collection_pid
 * @param array $expeditions
 * @param array $expeditions
 */
function si_relate_collection_to_expeditions($collection_pid, $expeditions) {
  $collection_object = new Fedora_Item($collection_pid);
  foreach ($expeditions as $expedition) {
    $query = 'select $object from <#ri> where $object <' . EAC_EXPEDITION_ID_NS . 'hasEacCpfExpedition> <info:fedora/' . $collection . '>';
    $results = si_run_query($query);
    $objects = si_sparql_results_as_array($results); //turn xml into array
    foreach ($objects as $exp)
      $collection_object->add_relationship('isMemberOfCollection', (string) $exp);
    drush_print('added expedition ' . (string) $exp . ' to ' . $collection_pid);
  }
}

/**
 *
 * @param SimpleXMLElement $content
 * @return array
 */
function si_sparql_results_as_array($content) {
  $content = new SimpleXMLElement($content);
  $resultsarray = array();
  foreach ($content->results->result as $result) {
    $resultsarray[] = substr($result->object->attributes()->uri, 12); // Remove 'info:fedora/'.
  }
  return $resultsarray;
}

/**
 *
 * @param strings $query
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
}

/**
 * the mods creator is just a place holder pointing to the Eaccpf record updating it here.
 * @param object $xml
 */
function si_update_mods_creator(&$xml) {
  $creators = $xml->xpath('//mods:role[mods:roleTerm = "creator"]/../../mods:name/mods:namePart');
  foreach ($creators as $creator) {
    $value = (string) $creator[0];
    if (!empty($value)) {
      $query = 'select $object from <#ri> where $object <' . EAC_PERSON_ID_NS . 'hasEacCpfPerson> <info:fedora/' . (string) $creator . '>';
      $results = si_run_query($query);
      $objects = si_sparql_results_as_array($results); //turn xml into array
      $item = new Fedora_Item((string) $objects[0]); //only one creator per mods creator entry
      $eac = $item->get_datastream_dissemination("EAC-CPF");
      if (empty($eac)) {
        $eac = $item->get_datastream_dissemination("EACCPF");
      }
      if (empty($eac)){
        drush_print("Error reading EAC-CPF stream");
        return;
      }
      $eac_xml = simplexml_load_string($eac);
      $namespaces = $eac_xml->getDocNamespaces();
      $eac_xml->registerXPathNamespace('eac', 'urn:isbn:1-931666-33-4');
      /* <cpfDescription>
         <identity>
         <entityType>person</entityType>
         <nameEntry localType="primary">
         <part localType="fullname">Abbott, William Louis, 1860-1936</part>
         </nameEntry> */
      $creator_text = $eac_xml->xpath('//eac:nameEntry[@localType="primary"]/eac:part');
      //$creator = (string)$creator_text[0];
      $dom_node = dom_import_simplexml($creator);
      $dom_node->nodeValue = (string) $creator_text[0];
    }
  }
  drush_print('mods  creator ' . (string) $result[0]);
}

function si_add_relationship_with_count($relationShip, $object, $uri) {

}

