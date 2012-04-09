<?php

/*
 * This file should be put in the drush/commands/islandora directory of your drush install
 * (you may have to create the islandora directory
 * 
 * updates xacml policies on all excepts specified by an itql query. 
 * 
 * @todo: add a datastream parameter then we could pass a file and update either collection_policy
 * xacml policy or any other dsid that is xml and needs to be updated across a group of objects.
 */

//drush hook

function si_update_xacml_policies_drush_command() {
  $items = array();

  $items['si_update_xacml_policies'] = array(
    'description' => "updates or adds an xacml policy to all objects returned by a query",
    'arguments' => array(
      'xacml_policy_file' => 'The path to an XML file that contains the xacml policy',
      'query_file' => 'The path to a plain text file that contains an itql query',
      'interactive' => 'if TRUE then you will be asked to confirm the update for each object'
    ),
    'examples' => array(
      'drush si_update_xacml_policies /path/to/xacml_policy.xml /path/to/query.txt',
    ),
    'aliases' => array('siuxp'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
  );

  return $items;
}

//drush hook
function drush_si_update_xacml_policies($xacml_policy_file, $query_file, $interactive) {
  drush_print('Current working directory ' . getcwd());
  if (isset($xacml_policy_file)) {
    drush_print("path to file " . $xacml_policy_file . "\n");
  }
  else {
    drush_print("no path to xacml Policy found");
    return;
  }
  if (isset($query_file)) {
    drush_print(" using query file" . $query_file);
  }
  else {
    drush_print(" no query_file found");
    return;
  }
  si_update_xacml_policies_doAction($xacml_policy_file, $query_file, $interactive);
}

/**
 *
 * @param sring $xacml_policy_file
 * @param string $query_file
 * @param string $interactive
 * @return void
 */
function si_update_xacml_policies_doAction($xacml_policy_file, $query_file, $interactive) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $query = file_get_contents($query_file);
  drush_print($query);
  if (!isset($query)) {
    drush_die('no query found');
    return;
  }
  $results = si_update_xacml_policies_run_query($query);
  $objects = si_sparql_results_as_array($results);
  foreach ($objects as $object) {
    drush_print($object);
  }
  //drush_print("are you sure you want to delete ".count($objects)." objects");
  $num = count($objects);
  if (!drush_confirm(dt('are you sure you want to update @num objects?', array('@num' => $num)))) {
    // was: return drush_set_error('CORE_SYNC_ABORT', 'Aborting.');
    drush_die('Aborting.');
  }
  file_copy($xacml_policy_file, file_directory_path());
  $datastream_url = drupal_urlencode($xacml_policy_file);
  $url = file_create_url($datastream_url);

  if ($interactive == 'TRUE') {
    foreach ($objects as $object) {
      if (drush_confirm(dt('update this xacml @pid ?', array('@pid' => $object)))) {
        $item = new Fedora_Item($object);
        si_update_xacml_policy($item, $url);
      }
    }
  }
  else {
    foreach ($objects as $object) {
      $item = new Fedora_Item($object);
      si_update_xacml_policy($item, $url);
    }
  }
}

/**
 *
 * @param fedora_item $item
 * @param string $xacml_policy_file 
 */
function si_update_xacml_policy($item, $url) {

  if (isset($item->datastreams["POLICY"])) {
    drush_print("POLICY exists updating datastream");
    $datastream_info = $item->get_datastream_info("POLICY");
    $control_group = $datastream_info->datastream->controlGroup;
    if ((string)$control_group == 'X') {
      $content = file_get_contents($url);
      $result = $item->modify_datastream_by_value($content, 'POLICY', 'POLICY', 'text/xml',  FALSE,  'Modified by drush update xacml POLICY');
    }
    else {
      $result = $item->modify_datastream_by_reference($url, "POLICY", "POLICY", "text/xml", $force = FALSE, $logMessage = 'Modified by drush update xacml POLICY');
    }
  }
  else {
    drush_print("xacml POLICY does not exist adding datastream");
    $result = $item->add_datastream_from_url($url, "POLICY", "POLICY", "text/xml", 'M', "Added xacml POLICY via drush update");
  }
  if (!isset($result)) {
    drush_print('error updating/adding xacml POLICY datastream');
  }
}

/**
 *
 * @param strings $query
 * @return string
 */
function si_update_xacml_policies_run_query($query) {
  $results = si_run_query($query); //function in si_batch_ingest.drush.inc
  if (isset($results)) {//still doesn't tell us if these are valid results
    return $results;
  }
  else {
    drush_print('Error executing resource index query');
    return NULL;
  }
}

?>
