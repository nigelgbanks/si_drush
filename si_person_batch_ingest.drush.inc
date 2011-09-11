<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

//drush hook

function si_person_batch_ingest_drush_command() {
  $items = array();

  $items['si_person_batch_ingest'] = array(
    'description' => "reads an filemaker pro xml dump and creates eac-cpf records and then creates objects in fedorawith eac-cpf streams",
    'arguments' => array(
      'collection_pid' => 'The pid of the collection object, this is the collection the new objects will be related to',
      'relationship' => 'The relationship the objects have to the collection object.  For example isMemberOf is a common relationship.',
      'input_file' => 'The path to the files (absolute file system path, /var/www/http/drupal/sites/default/files).',
      'input_xsl' => 'The path to the xsl file used to tranform the input_file',
    ),
    'examples' => array(
      'drush si_person_batch_ingest_drush_command islandora:collection isMemberOfCollection /path/to/xml/file /path/to/xsl/file',
    ),
    'aliases' => array('sipersonbatch'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

//drush hook
function drush_si_person_batch_ingest($collection_pid, $relationship, $input_file, $input_xsl) {
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
  
  si_doAction($collection_pid, $relationship, $input_file, $input_xsl);
}

//just a function
function si_doAction($pid, $relationship, $input_file, $input_xsl) {
  drush_print("input path = " . $input_path);
  si_person_transform($input_file,$input_xsl);
}

function si_person_transform($xml_file, $xsl_file) {
   $xslt = new xsltProcessor;
$xslt->importStyleSheet(DomDocument::load($xsl_file));
$eac_cpf_records = $xslt->transformToXML(DomDocument::loadXML(file_get_contents($xml_file)));
}

function si_create_object($pid, $relationship, $tif_file, $webserver_path_to_file) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  //uses fedora settings taken for islandora drupal settings
  $new_pid = Fedora_Item::get_next_PID_in_namespace();//we did not define a namespace so it will use the defaul
  drush_print("creating object with pid " . $new_pid);
  $object = Fedora_Item::ingest_new_item($new_pid, 'A', $tif_file);//create a new skeleton object
  drush_print("created object $new_pid now adding datastreams");
  $file_prefix = substr($tif_file,0,strlen($tif_file)-4);
  if ($object->add_datastream_from_url(url("$webserver_path_to_file/$file_prefix.xml"), 'MODS', 'MODS', 'text/xml', 'M', 'added MODS Stream')) {
    drush_print('added MODS stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding MODS stream to ' . $new_pid." using url $webserver_path_to_file/$file_prefix.xml");
  }
  if ($object->add_datastream_from_url(url("$webserver_path_to_file/$file_prefix.tif"), 'TIF', 'TIF', 'image/tif', 'M', 'added TIF Stream')) {
    drush_print('added TIF stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding TIF stream to ' . $new_pid);
  }
  if ($object->add_datastream_from_url(url("$webserver_path_to_file/$file_prefix.jpg"), 'JPG', 'JPG', 'image/jpg', 'M', 'added JPG Stream')) {
    drush_print('added JPG stream to ' . $new_pid);
  }
  else {
    drush_print('Failed adding JPG stream to ' . $new_pid);
  }
  
}


function endswith($string, $test) {
  $strlen = strlen($string);
  $testlen = strlen($test);
  if ($testlen > $strlen)
    return false;
  return substr_compare($string, $test, -$testlen) === 0;
}

?>
