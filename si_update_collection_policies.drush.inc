<?php

/**
 * @file si_update_collection_policies.drush.inc
 */

/**
 * Implements hook_drush_command().
 */
function si_update_collection_policies_drush_command() {
  return array(
    'si_update_collection_policies' => array(
      'description' => "Updates or adds a collection policy to all objects returned by a query",
      'arguments' => array(
        'collection_policy_file' => 'The path to an XML file that contains the collection_policy',
        'query_file' => 'The path to a plain text file that contains an itql query',
        'interactive' => 'if TRUE then you will be asked to confirm the update for each object'
      ),
      'examples' => array(
        'drush is_update_collection_policies /path/to/collection_policy.xml /path/to/query.txt',
      ),
      'aliases' => array('siucp'),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // we can pass in users id on the command line using drush -u.
    )
  );
}

/**
 * Implements drush_hook().
 */
function drush_si_update_collection_policies($collection_policy_file, $query_file, $interactive) {
  drush_print('Current working directory ' . getcwd());
  if (isset($collection_policy_file)) {
    drush_print("path to file " . $collection_policy_file . "\n");
  }
  else {
    drush_print("no path to Collection Policy found");
    return;
  }
  if (isset($query_file)) {
    drush_print(" using query file" . $query_file);
  }
  else {
    drush_print(" no query_file found");
    return;
  }
  si_update_collection_policies_doAction($collection_policy_file, $query_file, $interactive);
}

/**
 *
 */
function si_update_collection_policies_doAction($collection_policy_file, $query_file, $interactive) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $query = file_get_contents($query_file);
  drush_print($query);
  if (!isset($query)) {
    drush_die('no query found');
    return;
  }
  $results = si_update_collection_policies_run_query($query);
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
  file_copy($collection_policy_file, file_directory_path());
  $datastream_url = drupal_urlencode($collection_policy_file);
  $url = file_create_url($datastream_url);

  if ($interactive == 'TRUE') {
    foreach ($objects as $object) {
      if (drush_confirm(dt('update this collection @pid ?', array('@pid' => $object)))) {
        $item = new Fedora_Item($object);
        si_update_collection_policy($item, $url);
      }
    }
  }
  else {
    foreach ($objects as $object) {
      $item = new Fedora_Item($object);
      si_update_collection_policy($item, $url);
    }
  }
}

/**
 *
 * @param fedora_item $item
 * @param string $collection_policy_file
 */
function si_update_collection_policy($item, $url) {

  if (isset($item->datastreams["COLLECTION_POLICY"])) {
    drush_print("COLLECTION_POLICY exists updating datastream");
    $datastream_info = $item->get_datastream_info("COLLECTION_POLICY");
    $control_group = $datastream_info->datastream->controlGroup;
    if ((string)$control_group == 'X') {
      $content = file_get_contents($url);
      $result = $item->modify_datastream_by_value($content, 'COLLECTION_POLICY', 'COLLECTION_POLICY', 'text/xml',  FALSE,  'Modified by drush update collection policy');
    }
    else {
      $result = $item->modify_datastream_by_reference($url, "COLLECTION_POLICY", "COLLECTION_POLICY", "text/xml", $force = FALSE, $logMessage = 'Modified by drush update collection policy');
    }
  }
  else {
    drush_print("COLLECTION_POLICY does not exist adding datastream");
    $result = $item->add_datastream_from_url($url, "COLLECTION_POLICY", "COLLECTION_POLICY", "text/xml", 'M', "Added Collection Policy via drush update collection");
  }
  if (!isset($result)) {
    drush_print('error updating/adding COLLECTION_POLICY datastream');
  }
}

/**
 *
 * @param strings $query
 * @return string
 */
function si_update_collection_policies_run_query($query) {
  $results = si_run_query($query); //function in si_batch_ingest.drush.inc
  if (isset($results)) {//still doesn't tell us if these are valid results
    return $results;
  }
  else {
    drush_print('Error executing resource index query');
    return NULL;
  }
}
