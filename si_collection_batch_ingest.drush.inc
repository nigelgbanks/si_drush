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
function drush_si_collection_batch_ingest($collection_pid, $relationship, $input_file, $input_xsl) {
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
  
  si_collection_doAction($collection_pid, $relationship, $input_file, $input_xsl);
}

//just a function
function si_collection_doAction($pid, $relationship, $input_file, $input_xsl) {
  drush_print("input path = " . $input_path);
  si_collection_transform($input_file,$input_xsl);
}

function si_collection_transform($xml_file, $xsl_file) {
  $xslt = new xsltProcessor;
  $xslt->importStyleSheet(DomDocument::load($xsl_file));
  $ncd_records = $xslt->transformToXML(DomDocument::loadXML(file_get_contents($xml_file)));
  print_r($ncd_records);
}




?>
