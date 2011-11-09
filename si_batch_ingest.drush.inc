<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

//drush hook
define('EAC_PERSON_ID_NS', 'info:islandora/islandora-system:def/eacPersonId#');
define('EAC_EXPEDITION_ID_NS', 'info:islandora/islandora-system:def/eacExpeditionId#');
define('EAC_ORGANIZATION_ID_NS', 'info:islandora/islandora-system:def/eacOrganizationId#');
define('NCD_COLLECTION_ID_NS', 'info:islandora/islandora-system:def/ncdCollectionId#');

function si_batch_ingest_drush_command() {
  $items = array();

  $items['si_batch_ingest'] = array(
    'callback' => 'drush_si_batch_ingest',
    'description' => "reads an filemaker pro xml dump and creates fedora objects.  Valid types are person, expedition, organization, item and collection",
    'arguments' => array(
      'collection_pid' => 'The pid of the collection object, this is the collection the new objects will be related to',
      'relationship' => 'The relationship the objects have to the collection object.  For example isMemberOf is a common relationship.',
      'input_file' => 'The path to the files (absolute file system path, /var/www/http/drupal/sites/default/files).',
      'input_xsl' => 'The path to the xsl file used to tranform the input_file',
      'url_to_tn' => 'The url to the default thumbnail for this type of object',
      'type' => 'the type of object'
    ),
    'examples' => array(
      'drush si_batch_ingest_drush_command islandora:collection isMemberOfCollection /path/to/xml/file /path/to/xsl/file url_for_tn type',
    ),
    'aliases' => array('sibatch'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

/**
 *
 * @param <type> $collection_pid
 * @param <type> $relationship
 * @param <type> $input_file
 * @param <type> $input_xsl
 * @param <type> $url_to_tn
 * @return <type>
 */
function drush_si_batch_ingest($collection_pid, $relationship, $input_file, $input_xsl, $url_to_tn, $type) {
  module_load_include('inc', 'fedora_repository', 'CollectionClass');
  drush_print('Current working directory ' . getcwd());
  if (isset($collection_pid)) {
    drush_print("\n Used collection pid of" . $collection_pid . "\n");
  }
  else {
    drush_print("no collection pid specified");
    return;
  }
  if (isset($relationship)) {
    drush_print("\n and relationship of" . $relationship);
  }
  else {
    drush_print("\n no relationship specified");
    return;
  }
  if (!isset($input_file)) {
    drush_print("\n no input xml file specified");
    return;
  }
  if (!isset($input_xsl)) {
    drush_print("\n no input xsl file specified");
    return;
  }
  if (!isset($url_to_tn)) {
    drush_print("\n no thumbnail url specified");
    return;
  }
  if (!isset($type)) {
    drush_print("\n no object type specified");
    return;
  }

  si_doAction($collection_pid, $relationship, $input_file, $input_xsl, $url_to_tn, $type);
}

/**
 *
 * @param <type> $collection_pid
 * @param <type> $relationship
 * @param <type> $input_file
 * @param <type> $input_xsl
 * @param <type> $url_to_tn
 */
function si_doAction($collection_pid, $relationship, $input_file, $input_xsl, $url_to_tn, $type) {
  $xml_record = si_transform($input_file, $input_xsl);
  $type = (string) trim($type);
  $object = null;
  if ($type == 'collection') {//NCD schema
    $xml_records = explode("</RecordSet>", $xml_record); //they gave us all the records on one file so the tranform contains all the records as well simplexml will not load it as it is many valid docs contained in one
    foreach ($xml_records as $record) {
      $record = $record . '</RecordSet>';
      si_collection_create_object($collection_pid, $relationship, $record, $url_to_tn);
    }
  }
  else if ($type == 'item') {//transform is to mods and we need to update relationships across all types from data in this record
    $xml_records = explode("</mods>", $xml_record); //they gave us all the records in one file so the tranform contains all the records and simplexml will not load it as it is many valid docs contained in one so we have to split it up
    $orginal_records = simplexml_load_file($input_file); // there is content in the orginal that does not map to mods but we need to manage relationships
    $i = 0;
    foreach ($xml_records as $record) {
      $record = $record . '</mods>';
      $xml = simplexml_load_string($record);
      $namespaces = $xml->getDocNamespaces();
      $xml->registerXPathNamespace('mods', $namespaces['']);
      //TODO: implement a function to update the mods with human readable content based on the ids that exist in the data.
      ///mods:mods/mods:name[1]/mods:namePart[1]
      $or = $orginal_records->ROW[$i++]; //this should give us the corresponding orginal record
      $or = $or->asXML(); //make this valid xml so we can do xpath
      $or_xml = simplexml_load_string($or);
      //print_r($or);
      $relationship_data = si_get_fmp_relationship_data($or_xml); //get an array of relationship info to update relationships and update the mods so it is more meaningful
      $object = si_item_create_object($collection_pid, $relationship, $xml, $url_to_tn);
      //$object->add_relationship('hasEacCpfExpedition', $eac_id, 'info:islandora/islandora-system:def/eacItemId#');
      $object->add_relationship('hasModel', 'si:fieldbookCModel', FEDORA_MODEL_URI);
      ///mods:mods/mods:abstract[1]
      $description = $xml->xpath('//mods:abstract');
      update_dc($object, $description[0]);
      si_updateAllRelationships($object, $relationship_data); //update relationships based on the info in this record
    }
  }
  else {//all other types are eac-cpf
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
      update_dc($object, (string) $result[0]);
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
  }
}

/**
 * reads the orginal xml before we tranform to mods
 * gets the fields that hold relationhsip info and returns
 * an array contain this information
 * @param string $or
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
      if(!empty($relationship_data['expedition'])) {
        si_relate_collection_to_expeditions($collection, $relationship_data['expedition']);
      }
    }
  }
}

/**
 *
 * @param <type> $collection_object
 * @param array $expeditions
 */
function si_relate_collection_to_expeditions($collection_pid, $expeditions) {
  $collection_object = new Fedora_Item($collection_pid);
  foreach ($expeditions as $expedition) {
    $query = 'select $object from <#ri> where $object <' . EAC_EXPEDITION_ID_NS . 'hasEacCpfExpedition> <info:fedora/' . $collection . '>';
    $results = si_run_query($query);
    $objects = si_sparql_results_as_array($results); //turn xml into array
    foreach($objects as $exp)
    $collection_object->add_relationship('isMemberOfCollection', (string) $exp);
    drush_print('added expedition '.(string)$exp. ' to '. $collection_pid);
  }
  
}

/**
 *
 * @param SimpleXMLElement $content
 * @return <type>
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
 * @param <type> $query
 */
function si_run_query($query) {
  module_load_include('inc', 'fedora_repository', 'CollectionClass');
  $cc = new CollectionClass();
  $results = $cc->getRelatedItems(NULL, $query);
  if (isset($results)) {//still doesn't tell us if these are valid results
    return $results;
  }
  else {
    drush_print('Error get related items, relationships cannot be updated');
  }
}

/**
 *
 * @param <type> $pid
 * @param <type> $relationship
 * @param <type> $xml_file
 * @param <type> $xsl_file
 * @param <type> $url_to_tn
 * @param <type> $delimiter
 */
function si_transform($xml_file, $xsl_file) {
  $xslt = new xsltProcessor;
  $xslt->importStyleSheet(DomDocument::load($xsl_file));
  $records = $xslt->transformToXML(DomDocument::loadXML(file_get_contents($xml_file)));
  return $records;
}

/**
 *
 * @param <type> $collection_pid
 * @param <type> $relationship
 * @param <type> $xml
 * @param <type> $url_to_tn
 */
function si_create_object($collection_pid, $relationship, &$xml, $url_to_tn) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $result = $xml->xpath('//ns:identity/ns:nameEntry[@localType="primary"]/ns:part');
  $label = (string) $result[0];
  $new_pid = Fedora_Item::get_next_PID_in_namespace('si'); //we did not define a namespace so it will use the defaul
  drush_print("creating object with pid " . $new_pid . ' with label ' . $label);
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label); //create a new skeleton object
  drush_print("created object $new_pid now adding datastreams");
  if ($object->add_datastream_from_url($url_to_tn, 'TN', 'TN', 'image/png', 'M', 'Added TN Stream')) {
    drush_print('added TN stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TN stream to ' . $new_pid . " using url $url_to_tn");
  }
  if ($object->add_datastream_from_string($xml->asXML(), 'EAC-CPF', 'EAC-CPF', 'text/xml', 'M', 'added EAC-CPF Stream')) {
    drush_print('added EAC-CPF stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding EAC-CPF stream to ' . $new_pid);
  }
  $object->add_relationship($relationship, $collection_pid); //add_relationship always returns void so no sense checking

  return $object;
}

/**
 *
 * @param string $collection_pid
 * @param string $relationship
 * @param string $xml
 * @param string $url_to_tn
 */
function si_item_create_object($collection_pid, $relationship, &$xml, $url_to_tn) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $result = $xml->xpath('//mods:titleInfo/mods:title');
  $label = (string) $result[0];
  $new_pid = Fedora_Item::get_next_PID_in_namespace('si');
  drush_print("creating object with pid " . $new_pid . ' with label ' . $label);
  if(strlen($label)>255){
    $label = substr($label,0,254);
  }
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label); //create a new skeleton object
  drush_print("created object $new_pid now adding datastreams");
  if ($object->add_datastream_from_url($url_to_tn, 'TN', 'TN', 'image/png', 'M', 'Added TN Stream')) {
    drush_print('added TN stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TN stream to ' . $new_pid . " using url $url_to_tn");
  }
  if ($object->add_datastream_from_string($xml->asXML(), 'MODS', 'MODS', 'text/xml', 'M', 'added EAC-CPF Stream')) {
    drush_print('added MODS stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding MODS stream to ' . $new_pid);
  }
  $object->add_relationship($relationship, $collection_pid); //add_relationship always returns void so no sense checking

  return $object;
}

/**
 * 
 * @param <type> $collection_pid
 * @param <type> $relationship
 * @param <type> $ncd_xml
 * @param <type> $url_to_tn
 * @return <type> 
 */
function si_collection_create_object($collection_pid, $relationship, $ncd_xml, $url_to_tn) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  //uses fedora settings taken for islandora drupal settings
  $xml = simplexml_load_string($ncd_xml);
  //TODO:check for errors loading xml
  ///default:RecordSet/default:Collections[1]/default:Collection[1]/default:DesciptiveGroup[1]/default:dc_title[1]
  $namespaces = $xml->getDocNamespaces();
  $xml->registerXPathNamespace('__empty_ns', $namespaces['']);
  $result = $xml->xpath('//__empty_ns:dc_title');
  $label = (string) $result[0];
  $result = $xml->xpath('//__empty_ns:CollectionId');
  $ncd_id = (string) $result[0]; //we want to add this to the triple store so we can query when ingesting other objects
  drush_print("ncd id = $ncd_id");
  $new_pid = Fedora_Item::get_next_PID_in_namespace("si");
  drush_print("creating object with pid " . $new_pid . ' with label ' . $label);
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label); //create a new skeleton object
  drush_print("created object $new_pid now adding datastreams");
  if ($object->add_datastream_from_url($url_to_tn, 'TN', 'TN', 'image/png', 'M', 'Added TN Stream')) {
    drush_print('added TN stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TN stream to ' . $new_pid . " using url $url_to_tn");
  }
  if ($object->add_datastream_from_string($ncd_xml, 'NCD', 'NCD', 'text/xml', 'M', 'added NCD Stream')) {
    drush_print('added NCD stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding NCD stream to ' . $new_pid);
  }
  if ($object->add_relationship($relationship, $collection_pid)) {//add_relationship always returns void so no sense checking
    drush_print('added relationship ');
  }
  else {
    drush_print('Failed adding relationship ');
  }
  $object->add_relationship('hasModel', 'si:ncdCollectionCModel', FEDORA_MODEL_URI);

  $object->add_relationship('hasNcdId', $ncd_id, NCD_COLLECTION_ID_NS);
  $result = $xml->xpath('//__empty_ns:dc_description');
  update_dc($object, (string) $result[0]);
  return $object;
}

/**
 *
 * @param <type> $object
 * @param <type> $description
 * @param <type> $title optionally set the title as well
 */
function update_dc($object, $description) {
  module_load_include('inc', 'fedora_repository', 'api/dublin_core');
  $dc = new Dublin_Core($object);
  //$dc->remove_element("dc:description");
  if (isset($description)) {
    $dc->set_element('dc:descripton', $description);
  }
  else {
    drush_print("error adding description to dc, description not set.");
  }
  $dc->save();
}

?>
