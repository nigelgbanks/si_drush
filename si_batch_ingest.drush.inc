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
    ),
    'si_batch_test' => array(
      'callback' => 'drush_si_batch_test',
      'description' => "Ingests all fieldbook related data, make sure the proper data is in the drush directory.",
      'examples' => array(
        'drush ingest-fieldbooks',
      ),
      'aliases' => array('si_test'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // We can pass in users id on the command line using drush -u.
    )
  );
}


function drush_si_batch_test() {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  drush_print('testing');
  $query = 'select $object from <#ri> where
$object <fedora-model:hasModel> <info:fedora/si:ctPlotCModel> or
$object <fedora-model:hasModel> <info:fedora/si:cameraTrapCModel> or
$object <fedora-model:hasModel> <info:fedora/si:organizationCModel> or
$object <fedora-model:hasModel> <info:fedora/si:peopleCModel> or
$object <fedora-model:hasModel> <info:fedora/si:projectCModel> or
$object <fedora-model:hasModel> <info:fedora/si:expeditionCModel> or
$object <fedora-model:hasModel> <info:fedora/si:ncdCollectionCModel> or
$object <fedora-model:hasModel> <info:fedora/si:projectCModel> or
$object <fedora-model:hasModel> <info:fedora/si:lidoCollectionCModel> or
$object <fedora-model:hasModel> <info:fedora/si:dwcCModel> or
$object <fedora-model:hasModel> <info:fedora/si:collectionCModel>';
  $results = si_run_query($query);
  $objects = si_sparql_results_as_array($results); //turn xml into array
  foreach ($objects as $pid) {
    $item = new Fedora_Item($pid);
    if (!isset($item->datastreams['COLLECTION_POLICY'])) {
      drush_print("Adding COLLECTION_POLICY: $pid");
      si_add_collection_policy($item);
    }
    else {
      drush_print("Updating COLLECTION_POLICY: ($pid)");
      si_update_collection_policy_new($item);
    }
  }
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
function si_ingest_records(array $records, $function) {
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
  $records = si_transform('persons.xml', 'EAC_person.xsl', 'eac-cpf');
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
function si_ingest_person($record) {
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
  $records = si_transform('organizations.xml', 'EAC_organization.xsl', 'eac-cpf');
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
function si_ingest_organization($record) {
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
  $records = si_transform('expeditions.xml', 'EAC_expedition.xsl', 'eac-cpf');
  return si_ingest_records($records, 'si_ingest_expedition');
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
function si_ingest_expedition($record) {
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
  $records = si_transform('collections.xml', 'NCD_collection.xsl', 'RecordSet');
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
function si_ingest_collection($record) {
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
  $records = si_transform('fieldbooks.xml', 'MODS_fieldbook.xsl', 'mods');
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
function si_ingest_fieldbook($record) {
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
  $records = $xslt->transformToXML(DOMDocument::loadXML(file_get_contents("$directory/data/fieldbooks/$input")));
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
 * Adds a COLLECTION_POLICY datastream to the object.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 */
function si_add_collection_policy(Fedora_Item $object) {
  $directory = dirname(__FILE__);
  si_add_datastream_from_file($object, 'COLLECTION_POLICY', "$directory/data/datastreams/COLLECTION_POLICY.xml", 'text/xml');
}

/**
 * Adds a COLLECTION_POLICY datastream to the object.
 *
 * @param Fedora_Item $object
 *   The fedora object to add the thumbnail to.
 */
function si_update_collection_policy_new(Fedora_Item $object) {
  $directory = dirname(__FILE__);
  $xml = new DOMDocument();
  $datastream = $object->get_datastream('COLLECTION_POLICY');

  $xml->load("$directory/data/datastreams/COLLECTION_POLICY.xml");
  $object->modify_datastream_by_value($xml->saveXML(), 'COLLECTION_POLICY', 'COLLECTION_POLICY', 'text/xml');
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
function si_add_relation_literal(Fedora_Item $object, $predicate, $literal, $uri) {
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
function si_eaccpf_get_info($record) {
  $xml = si_get_simple_xml($record);
  $functions = array(
    TYPE_PERSON => 'si_eaccpf_person_get_info',
    TYPE_ORGANIZATION => 'si_eaccpf_organization_get_info',
    TYPE_EXPEDITION => 'si_eaccpf_expedition_get_info',
  );
  $id = si_get_value_from_xml($xml, '//ns:recordId');
  $function = $functions[si_get_type_from_object_id($id)]; // Specialization of EAC-CPF type.
  return array_merge(array(
      'id' => $id,
      'label' => si_normalize_object_label(si_get_value_from_xml($xml, '//ns:identity/ns:nameEntry[@localType="primary"]/ns:part')),
      'description' => si_get_value_from_xml($xml, '//ns:biogHist/ns:p'),
      'cpf_relations' => si_get_values_from_xml($xml, '//ns:cpfRelation/@xlink:href'),
      'resource_relations' => si_get_values_from_xml($xml, '//ns:resourceRelation/@xlink:href'),
    ), $function($xml));
}

/**
 * Gets the info that is unique to this kind of EAC-CPF record
 */
function si_eaccpf_person_get_info(SimpleXMLElement $xml) {
  return array(
    'related_material' => si_get_values_from_xml($xml, '//ns:resourceRelation[ns:relationEntry/@localType="material"]/@xlink:href'),
    'related_entities' => si_get_values_from_xml($xml, '//ns:cpfRelation[ns:relationEntry/@localType="entity"]/@xlink:href'),
  );
}

/**
 * Gets the info that is unique to this kind of EAC-CPF record
 */
function si_eaccpf_organization_get_info(SimpleXMLElement $xml) {
  return array(
    'parent' => si_get_value_from_xml($xml, '//ns:cpfRelation[@cpfRelationType="hierarchical-parent"]/@xlink:href'),
  );
}

/**
 * Gets the info that is unique to this kind of EAC-CPF record
 */
function si_eaccpf_expedition_get_info(SimpleXMLElement $xml) {
  return array(
    'participant_organizations' => si_get_values_from_xml($xml, '//ns:cpfRelation[ns:relationEntry/@localType="organization"]/@xlink:href'),
    'participant_persons' => si_get_values_from_xml($xml, '//ns:cpfRelation[ns:relationEntry/@localType="person"]/@xlink:href'),
    'related_material' => si_get_values_from_xml($xml, '//ns:resourceRelation[ns:relationEntry/@localType="material"]/@xlink:href'),
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
function si_ncd_get_info($record) {
  $xml = si_get_simple_xml($record);
  $expeditions = si_get_values_from_xml($xml, '//ns:ExpeditionName');
  $expeditions = array_filter($expeditions, 'si_is_id');
  $collectors = si_get_values_from_xml($xml, '//ns:Collector');
  $collectors = array_filter($collectors, 'si_is_id');
  $associated_persons = si_get_values_from_xml($xml, '//ns:AssociatedAgent');;
  $associated_persons = array_filter($associated_persons, 'si_is_id');
  $related_collections = si_get_values_from_xml($xml, '//ns:RelatedMaterialsGroup/ns:dc_relation');;
  $related_collections = array_filter($related_collections, 'si_is_id');
  return array(
    'id' => si_get_value_from_xml($xml, '//ns:CollectionId'),
    'alt_id' => si_get_value_from_xml($xml, '//ns:AlternativeIds/ns:Identifier[1]'),
    'label' => si_normalize_object_label(si_get_value_from_xml($xml, '//ns:dc_title')),
    'description' => si_get_value_from_xml($xml, '//ns:dc_description'),
    'expeditions' => $expeditions,
    'collectors' => $collectors,
    'associated_persons' => $associated_persons,
    'related_collections' => $related_collections,
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
function si_mods_get_info($record) {
  $xml = si_get_simple_xml($record);
  return array(
    'id' => si_get_value_from_xml($xml, '/ns:mods/@ID'),
    'alt_id' => si_get_value_from_xml($xml, '/ns:mods/ns:identifier[@type="accessionNumber"]'),
    'label' => si_normalize_object_label(si_get_value_from_xml($xml, '//ns:titleInfo/ns:title')),
    'description' => si_get_value_from_xml($xml, '//ns:titleInfo/ns:abstract'),
    'related_names' => si_get_values_from_xml($xml, '//ns:name/@valueURI'),
    'related_items' => si_get_values_from_xml($xml, '//ns:relatedItem/ns:titleInfo/@valueURI'),
    'collection' => si_get_value_from_xml($xml, '//nd:relatedItem[@type="host"]/ns:identifier'),
    'creators' => si_get_values_from_xml($xml, '//ns:name[ns:role/ns:roleTerm="creator"]/@valueURI'),
    'contributors' => si_get_values_from_xml($xml, '//ns:name[ns:role/ns:roleTerm="contributor"]/@valueURI'),
    'expeditions' => si_get_values_from_xml($xml, '//ns:subject/ns:name[@displayLabel="Expedition"]/@valueURI'),
    'organizations' => si_get_values_from_xml($xml, '//ns:subject/ns:name[@displayLabel="Organization"]/@valueURI'),
    'persons' => si_get_values_from_xml($xml, '//ns:subject/ns:name[@displayLabel="Person"]/@valueURI'),
  );
}

/**
 * Gets the string as a simple xml object, with 'ns' as the default namespace.
 */
function si_get_simple_xml($record) {
  $xml = simplexml_load_string($record);
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
 * Updates the given object using data stored in other objects.
 *
 * @param array $object
 *   Properties that define a object to update.
 * @param string $type
 *   The TYPE of the object properties as defined at the top of this file.
 * @param array $objects
 *   Properties that define all the objects.
 */
function si_update_object(array $object, $type, array $objects) {
  static $update_functions = array(
    TYPE_PERSON => 'si_update_person',
    TYPE_ORGANIZATION => 'si_update_organization',
    TYPE_EXPEDITION => 'si_update_expedition',
    TYPE_COLLECTION => 'si_update_collection',
    TYPE_FIELDBOOK => 'si_update_fieldbook'
  );
  $function = $update_functions[$type];
  drush_print("Updating object: {$object['label']} ({$object['pid']}/{$object['id']})");
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
  si_update_eaccpf($object, $objects);
  si_add_collection_policy_to_object($object);
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
  si_update_eaccpf($object, $objects);
  si_update_relationship($object, $object['parent'], 'isMemberOfCollection', RELS_EXT_URI, $objects);
  si_add_collection_policy_to_object($object);
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
  si_update_eaccpf($object, $objects);
  si_add_collection_policy_to_object($object);
}

/**
 * Updates and EAC-CPF record.
 */
function si_update_eaccpf(array $object, array $objects) {
  list($item, $dsid, $xml) = si_get_objects_item_and_metadata($object);
  si_update_references_with_label($xml, $object['cpf_relations'], '//ns:cpfRelation[@xlink:href="%id"]/ns:relationEntry', $objects);
  si_update_references_with_label($xml, $object['resource_relations'], '//ns:resourceRelation[@xlink:href="%id"]/ns:relationEntry', $objects);
  $item->modify_datastream_by_value($xml->asXML(), $dsid, $dsid, 'text/xml');
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
  // Collections have no place to store the ID's
  si_add_collection_policy_to_object($object);
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
  list($item, $dsid, $xml) = si_get_objects_item_and_metadata($object);
  si_update_references_with_label($xml, $object['related_names'], '//ns:name[@valueURI="%id"]/ns:namePart', $objects);
  si_update_references_with_label($xml, $object['related_items'], '//ns:relatedItem/ns:titleInfo[@valueURI="%id"]/ns:title', $objects);
  $item->modify_datastream_by_value($xml->asXML(), $dsid, $dsid, 'text/xml');
  si_update_relationships($object, $object['creators'], 'isMemberOfCollection', RELS_EXT_URI, $objects); // People
  si_update_relationships($object, $object['expeditions'], 'isMemberOfCollection', RELS_EXT_URI, $objects); // Expeditions
  si_update_relationship($object, $object['collection'], 'isMemberOfCollection', RELS_EXT_URI, $objects); // Collection
}

/**
 * Fetchs the object and its metadata.
 */
function si_get_objects_item_and_metadata(array $object) {
  module_load_include('module', 'fedora_repository', 'api/fedora_item');
  $item = new Fedora_Item($object['pid']);
  $dsid = si_get_dsid_from_object_id($object['id']);
  $ds = $item->get_datastream_dissemination($dsid);
  $xml = simplexml_load_string($ds);
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('ns', $namespaces['']);
  return array($item, $dsid, $xml);
}

/**
 * Updates the references found at $xpath.
 */
function si_update_references_with_label(SimpleXMLElement &$xml, array $ids, $xpath, array $objects) {
  array_walk(si_find_objects($ids, $objects), function($object) use(&$xml, $xpath) {
      $label = $object['label'];
      $query = str_replace('%id', $object['id'], $xpath);
      $elements = $xml->xpath($query);
      foreach($elements as $element) {
        $element->{0} = $label;
      }
  });
}

/**
 * Creates relationships between all $ids.
 */
function si_update_relationships(array $object, array $ids, $predicate, $uri, array $objects) {
  module_load_include('module', 'fedora_repository', 'api/fedora_item');
  $item = new Fedora_Item($object['pid']);
  array_walk(si_find_objects($ids, $objects), function($related_object) use($item, $predicate, $uri) {
      $item->add_relationship($predicate, $related_object['pid'], $uri);
  });
}

/**
 * Adds a COLLECTION_POLICY to the given object.
 */
function si_add_collection_policy_to_object(array $object) {
  list($item, $dsid, $xml) = si_get_objects_item_and_metadata($object);
  si_add_collection_policy($item);
}

/**
 * Updates a single relationship
 */
function si_update_relationship(array $object, $id, $predicate, $uri, array $objects) {
  si_update_relationships($object, array($id), $predicate, $uri, $objects);
}

/**
 * Finds all the objects identified by $ids
 */
function si_find_objects(array $ids, array $objects) {
  $ret = array();
  foreach($ids as $id) {
    if($object = si_find_object($id, $objects)) {
      $ret[] = $object;
    }
  }
  return $ret;
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
function si_find_object($id, array $objects) {
  $type = si_get_type_from_object_id($id);
  $objects = $objects[$type];
  if(!empty($objects)) {
    $result = array_filter($objects, function($object) use($id) {
                return $object['id'] == $id;
              });
    return empty($result) ? FALSE : array_shift($result);
  }
  return FALSE;
}

/**
 * Returns the type for the ID.
 */
function si_get_type_from_object_id($id) {
  $types = array(TYPE_PERSON, TYPE_ORGANIZATION, TYPE_EXPEDITION, TYPE_COLLECTION, TYPE_FIELDBOOK);
  foreach($types as $type) {
    if(strpos($id, $type) !== FALSE) {
      return $type;
    }
  }
}

/**
 * Checks if the given string is an ID.
 */
function si_is_id($id) {
  $types = array(TYPE_PERSON, TYPE_ORGANIZATION, TYPE_EXPEDITION, TYPE_COLLECTION, TYPE_FIELDBOOK);
  foreach($types as $type) {
    if(strpos($id, $type) !== FALSE) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Gets a destined DSID for the given type.
 */
function si_get_dsid_from_object_id($id) {
  $dsid = array(TYPE_PERSON => 'EAC-CPF', TYPE_ORGANIZATION => 'EAC-CPF', TYPE_EXPEDITION =>'EAC-CPF', TYPE_COLLECTION => 'NCD', TYPE_FIELDBOOK => 'MODS');
  $type = si_get_type_from_object_id($id);
  return $dsid[$type];
}


/**
 *
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