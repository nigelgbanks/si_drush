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
$object <fedora-model:hasModel> <info:fedora/si:ctCollectionCModel> or
$object <fedora-model:hasModel> <info:fedora/si:collectionCModel>';
  $results = si_run_query($query);
  $objects = si_sparql_results_as_array($results); //turn xml into array
  /*
  foreach ($objects as $pid) {
    $item = new Fedora_Item($pid);
    if (!isset($item->datastreams['COLLECTION_POLICY'])) {
      drush_print("Adding COLLECTION_POLICY: $pid");
      si_add_collection_policy($item);
    }
    else {
      drush_print("Updating COLLECTION_POLICY: ($pid)");
      si_update_collection_policy($item);
    }
    }*/
}

/**
 * Generate/Ingest all field book related data. Also makes relevant relationships between the ingested data.
 */
function drush_si_batch_ingest_fieldbooks() {
  drush_print('Batch Ingesting Fieldbooks into Fedora repository: ' . variable_get('fedora_base_url', 'http://localhost:8080/fedora'));
  si_utils_drush_init(); // Initialize utils file.
  $ingest_functions = array(
    TYPE_PERSON => 'si_ingest_person',
    TYPE_ORGANIZATION => 'si_ingest_organization',
    TYPE_EXPEDITION => 'si_ingest_expedition',
    TYPE_COLLECTION => 'si_ingest_collection',
    TYPE_FIELDBOOK => 'si_ingest_fieldbook'
  );
  $xml_records = array(
    TYPE_PERSON => si_fieldbook_transform_records('persons.xml', 'EAC_person.xsl', 'eac-cpf'),
    TYPE_ORGANIZATION => si_fieldbook_transform_records('organizations.xml', 'EAC_organization.xsl', 'eac-cpf'),
    TYPE_EXPEDITION => si_fieldbook_transform_records('expeditions.xml', 'EAC_expedition.xsl', 'eac-cpf'),
    TYPE_COLLECTION => si_fieldbook_transform_records('collections.xml', 'NCD_collection.xsl', 'RecordSet'),
    TYPE_FIELDBOOK => si_fieldbook_transform_records('fieldbooks.xml', 'MODS_fieldbook.xsl', 'mods')
  );
  $update_functions = array(
    TYPE_PERSON => 'si_update_person',
    TYPE_ORGANIZATION => 'si_update_organization',
    TYPE_EXPEDITION => 'si_update_expedition',
    TYPE_COLLECTION => 'si_update_collection',
    TYPE_FIELDBOOK => 'si_update_fieldbook'
  );
  si_batch_update($update_functions, si_batch_ingest($ingest_functions, $xml_records));
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
 * Transforms the given input with the given tranformation.
 *
 * The given files are expected to live in the data/xsl directories respectively.
 *
 * @param string $file
 *   The file to be tranformed.
 * @param string $tranform to be applied.
 *   The xsl transform to apply
 * @param string $root_element
 *   The case-sensitive name of the root element in the transformed records.
 *
 * @returns array
 *   Each record as a string.
 */
function si_fieldbook_transform_records($file, $transform, $root_element) {
  $directory = dirname(__FILE__);
  $input = si_transform_xml_file("$directory/data/fieldbooks/$file", "$directory/xsl/$transform", $root_element);
  return si_split_xml_file($input, $root_element);
}

/**
 * Batch ingest all the availible objects.
 *
 * @param array $ingest_functions
 *   An array of key values pairs where each key is a group of objects, and each value is a
 *   function that returns ingested objects that belong to that group.
 * @param array $records
 *   An array of key values pairs where each key is a group of objects, and each value is a
 *   array of xml records to be ingested.
 *
 * @return array
 *   An array of all the objects created, grouped by the types defined at the top of this file.
 */
function si_batch_ingest(array $ingest_functions, array $records) {
  $objects = array();
  foreach($ingest_functions as $key => $function) {
    foreach($records[$key] as $record) {
      $objects[$key][] = $function($record);
    }
  }
  $count = array_reduce($objects, function($result, $object_group) { return $result + count($object_group); }, 0);
  drush_print("Ingested $count Objects");
  return $objects;
}

/**
 * Batch update all the ingested objects. Sometimes we don't have the all the information required for populating
 * an object's meta-data or data-streams until some of its related objects have been ingested.
 *
 * @param array $update_functions
 *   An array of key values pairs where each key is a group of objects, and each value is a
 *   function that updates objects within that group.
 * @param array $objects
 *   The objects to be updated.
 */
function si_batch_update(array $update_functions, array $objects) {
  foreach($update_functions as $key => $function) {
    foreach($objects[$key] as $object) {
      drush_print("Updating object: {$object['label']} ({$object['pid']}/{$object['id']})");
      $function($object, $objects);
    }
  }
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
  si_add_external_relation_literal($object, EACP_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
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
  si_add_external_relation_literal($object, EACO_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
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
  si_add_external_relation_literal($object, EACE_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
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
  si_add_external_relation_literal($object, NCD_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  $object->forget(); // Prevent Memory Leaks
  return $data;
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
  si_add_external_relation_literal($object, MODS_ID_PREDICATE, $data['id'], SMITHSONIAN_ONTOLOGY_URI);
  if(!empty($data['alt_id'])) {
    si_add_external_relation_literal($object, ALT_ID_PREDICATE, $data['alt_id'], SMITHSONIAN_ONTOLOGY_URI);
  }
  $object->forget(); // Prevent Memory Leaks
  return $data;
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
 * Adds a COLLECTION_POLICY to the given object.
 */
function si_add_collection_policy_to_object(array $object) {
  list($item, $dsid, $xml) = si_get_objects_item_and_metadata($object);
  si_add_collection_policy($item);
}

/**
 * Creates relationships between all $ids.
 */
function si_update_relationships(array $object, array $ids, $predicate, $uri, array $objects) {
  $item = new Fedora_Item($object['pid']);
  array_walk(si_find_objects($ids, $objects), function($related_object) use($item, $predicate, $uri) {
      $item->add_relationship($predicate, $related_object['pid'], $uri);
  });
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
