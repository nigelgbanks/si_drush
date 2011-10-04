<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

//drush hook

function si_collection_batch_ingest_drush_command() {
  $items = array();

  $items['si_collection_batch_ingest'] = array(
    'callback' => 'drush_si_collection_batch_ingest',
    'description' => "reads an filemaker pro xml dump and creates eac-cpf records and then creates objects in fedorawith eac-cpf streams",
    'arguments' => array(
      'collection_pid' => 'The pid of the collection object, this is the collection the new objects will be related to',
      'relationship' => 'The relationship the objects have to the collection object.  For example isMemberOf is a common relationship.',
      'input_file' => 'The path to the files (absolute file system path, /var/www/http/drupal/sites/default/files).',
      'input_xsl' => 'The path to the xsl file used to tranform the input_file',
      'url_to_tn' => 'the url of the thumbnail image',
    ),
    'examples' => array(
      'drush si_collection_batch_ingest_drush_command islandora:collection isMemberOfCollection /path/to/xml/file /path/to/xsl/file',
    ),
    'aliases' => array('sincdcollectionbatch'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

//drush hook
function drush_si_collection_batch_ingest($collection_pid, $relationship, $input_file, $input_xsl, $url_to_tn) {
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
    drush_print("\n no input file specified");
    return;
  }
  if (!isset($input_xsl)) {
    drush_print("\n no input file specified");
    return;
  }

  si_collection_doAction($collection_pid, $relationship, $input_file, $input_xsl,$url_to_tn);
}

//just a function
function si_collection_doAction($pid, $relationship, $input_file, $input_xsl,$url_to_tn) {
  drush_print("input path = " . $input_path);
  si_collection_transform($pid, $relationship, $input_file, $input_xsl,$url_to_tn);
}

function si_collection_transform($pid, $relationship, $xml_file, $xsl_file,$url_to_tn) {
  $xslt = new xsltProcessor;
  $xslt->importStyleSheet(DomDocument::load($xsl_file));
  $ncd_records = $xslt->transformToXML(DomDocument::loadXML(file_get_contents($xml_file)));
  $xml_records = explode("</RecordSet>", $ncd_records); //they gave us all the records on one file so the tranform contains all the records as well simplexml will not load it as it is many valid docs contained in one
  foreach ($xml_records as $record) {
    $record = $record.'</RecordSet>';
    create_object($pid, $relationship, $record, $url_to_tn);
    //print_r($record.'</RecordSet>');
  }
}


function create_object($collection_pid, $relationship, $ncd_xml, $url_to_tn) {
module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  //uses fedora settings taken for islandora drupal settings
  $xml = simplexml_load_string($ncd_xml);
  //TODO:check for errors loading xml
  ///default:RecordSet/default:Collections[1]/default:Collection[1]/default:DesciptiveGroup[1]/default:dc_title[1]
   $namespaces = $xml->getDocNamespaces();
    $xml->registerXPathNamespace('__empty_ns', $namespaces['']);
    $result = $xml->xpath('//__empty_ns:dc_title');
  $label = (string)$result[0];
  $result = $xml->xpath('//__empty_ns:CollectionId');
  $ncd_id = (string)$result[0];//we want to add this to the triple store so we can query when ingesting other objects
  drush_print("ncd id = $ncd_id");
  $new_pid = Fedora_Item::get_next_PID_in_namespace();//we did not define a namespace so it will use the defaul
  drush_print("creating object with pid " . $new_pid. ' with label '.$label);
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $label);//create a new skeleton object
  drush_print("created object $new_pid now adding datastreams");
  if ($object->add_datastream_from_url($url_to_tn, 'TN', 'TN', 'image/png', 'M', 'Added TN Stream')) {
    drush_print('added TN stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TN stream to ' . $new_pid." using url $url_to_tn");
  }
  if ($object->add_datastream_from_string($ncd_xml, 'NCD', 'NCD', 'text/xml', 'M', 'added NCD Stream')) {
    drush_print('added TIF stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TIF stream to ' . $new_pid);
  }
  if ($object->add_relationship($relationship, $collection_pid)) {
    drush_print('added relationship ' );
  }
  else {
    drush_print('Failed adding relationship ');
  }
  $object->add_relationship('hasModel', 'si:ncdCollectionCModel',FEDORA_MODEL_URI);
  
  $object->add_relationship('hasNcdId', $ncd_id,'info:islandora/islandora-system:def/ncdCollectionId#');
 

}

?>
