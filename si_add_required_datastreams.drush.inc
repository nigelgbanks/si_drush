<?php

/**
 * @file
 *
 * There are occasions where objects get created without the required datastreams this file defines a batch
 * process that will generate stubbed values for those datastreams.
 */

/**
 * Defines the drush command for ingesting Fieldbook data.
 */
function si_add_required_datastreams_drush_command() {
  return array(
    'si_add_required_datastreams' => array(
      'callback' => 'drush_si_add_required_datastreams',
      'description' => "Adds all required datastreams to every smithsonian concept/resource object.",
      'examples' => array(
        'drush ingest-fieldbooks',
      ),
      'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN, // We can pass in users id on the command line using drush -u.
    )
  );
}

/**
 * Generate/Ingest all field book related data. Also makes relevant relationships between the ingested data.
 */
function drush_si_add_required_datastreams($pid = NULL) {
  $repo = si_get_tuque_fedora_repository();
  if(isset($pid)) {
    drush_print("Adding required datastreams to $pid");
    try {
      si_add_required_datastreams($repo->getObject($pid));
     } catch(Exception $e) {
      drush_print("An error occured while adding the required datastreams to $pid: " . $e->getMessage());
    }
  }
  else {
    $models = array(
      'si:imageCModel',
      'si:fieldbookCModel',
      'si:datasetCModel',
      'si:projectCModel',
      'si:ncdCollectionCModel',
      'si:ctPlotCModel',
      'si:peopleCModel',
      'si:organizationCModel',
      'si:expeditionCModel',
      'si:cameraTrapCModel',
      'si:dwcCModel',
      'si:lidoCollectionCModel',
      'si:collectionCModel'
    );
    foreach($models as $model) {
      $query = 'select $o from <#ri> where $o <fedora-model:hasModel> <info:fedora/' . $model . '>';
      $results = $repo->ri->itqlQuery($query);
      $results = array_map(function($o) { return $o['o']['value']; }, $results);
      foreach($results as $pid) {
        si_add_required_datastreams($repo->getObject($pid));
      }
    }
  }
}

/**
 * Validate the arguments provided to this drush command.
 */
function drush_si_add_required_datastreams_validate() {
  global $user;
  if($user->uid != 1) {
    return drush_set_error('INVALID_ARGUMENT', dt('Must be the admin user to run this command, specify "-u 1" as an argument.'));
  }
  $valid_uris = array('http://si-islandora.si.edu', 'http://si-islandora.si.edu/test');
  $cli = drush_get_context('cli');
  if(!isset($cli['uri'])) {
    return drush_set_error('INVALID_ARGUMENT', dt('The argument --uri is required. Please use either "http://si-islandora.si.edu" or "http://si-islandora.si.edu/test"'));
  }
  else if(array_search($cli['uri'], $valid_uris) === FALSE) {
    return drush_set_error('INVALID_ARGUMENT', dt('The --uri given is not valid. Please use either "http://si-islandora.si.edu" or "http://si-islandora.si.edu/test"'));
  }
}

/**
 * Adds the required datastreams to the given object. Required datastreams are defined by the objects content models.
 *
 * @param FedoraObject $o
 *   The object to add datastreams to.
 */
function si_add_required_datastreams(FedoraObject $o) {
 $path = dirname(__FILE__) . '/data';
 $requirements = array(
    'si:imageCModel' => array(
      'IMAGE' => SI_DATASTREAM_DIR . '/imageCModel/IMAGE.jpg',
      'DWC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/imageCModel/DWC.xsl'),
      'POLICY' => SI_DEFAULT_POLICY
    ),
    'si:fieldbookCModel' => array(
      'TN' => SI_DATASTREAM_DIR . '/fieldbookCModel/TN.png',
      'MODS' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/fieldbookCModel/MODS.xsl'),
      'POLICY' => SI_DEFAULT_POLICY
    ),
    'si:datasetCModel' => array(
      'FGDC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/datasetCModel/FGDC.xsl'),
      'OBJ' => SI_DATASTREAM_DIR . '/datasetCModel/csv.txt',
      'CSV' => SI_DATASTREAM_DIR . '/datasetCModel/csv.txt',
      'POLICY' => SI_DEFAULT_POLICY // Should we include a stub CSV
    ),
    'si:projectCModel' => array(
      'FGDC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/projectCModel/EAC-CPF.xsl'),
      'POLICY' => SI_DEFAULT_POLICY, // Should we include a stub CSV
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:ncdCollectionCModel' => array(
      'NCD' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/ncdCollectionCModel/NCD.xsl'),
      'POLICY' => SI_DEFAULT_POLICY, // Should we include a stub CSV
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:ctPlotCModel' => array(
      'FGDC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/ctPlotCModel/FGDC.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:peopleCModel' => array(
      'TN' => SI_DATASTREAM_DIR . '/peopleCModel/TN.png',
      'EAC-CPF' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/peopleCModel/EAC-CPF.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:organizationCModel' => array(
      'TN' => SI_DATASTREAM_DIR . '/organizationCModel/TN.png',
      'EAC-CPF' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/organizationCModel/EAC-CPF.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:expeditionCModel' => array(
      'TN' => SI_DATASTREAM_DIR . '/collectionCModel/TN.png',
      'EAC-CPF' => array('si_transform_xml_datastream', SI_DATASTREAM_DIR . '/expeditionCModel/EAC-CPF.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:cameraTrapCModel' => array(
      'FGDC' => array('si_transform_xml_datastream', SI_DATASTREAM_DIR . '/cameraTrapCModel/FGDC.xsl'),
      'CUSTOMCT' => array('si_transform_xml_datastream', SI_DATASTREAM_DIR . '/cameraTrapCModel/CUSTOMCT.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:dwcCModel' => array(
      'IMAGE' => SI_DATASTREAM_DIR . '/dwcCModel/IMAGE.jpg',
      'DWC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/dwcCModel/DWC.xsl'),
      'POLICY' => SI_DEFAULT_POLICY
    ),
    'si:lidoCollectionCModel' => array(
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    ),
    'si:collectionCModel' => array(
      'FGDC' => array('si_generate_xml_datastream_with_transform', SI_DATASTREAM_DIR . '/collectionCModel/FGDC.xsl'),
      'POLICY' => SI_DEFAULT_POLICY,
      'COLLECTION_POLICY' => SI_DEFAULT_COLLECTION_POLICY
    )
  );
  $results = $o->relationships->get('info:fedora/fedora-system:def/model#', 'hasModel');
  $models = array_map(function($o) { return $o['object']['value']; }, $results);
  $datastreams = array();
  foreach($models as $model) {
    if(isset($requirements[$model])) {
      $datastreams = array_merge($datastreams, $requirements[$model]);
    }
  }
  foreach($datastreams as $dsid => $datastream) {
    if(isset($o[$dsid])) {
      break; // Already Present
    }
    drush_print("Adding $dsid to ($o->pid) {$o->label}");
    $ds = $o->constructDatastream($dsid);
    if(is_array($datasteam)) { // Needs preprocessing
      $tmpfile = tempnam();
      list($function, $file) = $datastream;
      file_put_contents($tmpfile, $function($o, $file));
      $ds->setContentFromFile($tmpfile);
      $o->ingestDatastream($ds);
      unlink($tempfile);
    }
    else {
      $ds->setContentFromFile($datastream);
      $o->ingestDatastream($ds);
    }
  }
}

/**
 * Generates an XML datastream from a transform.
 *
 * @param FedoraObject $o
 *   The object in which the datastream will be added to.
 * @param string $transform
 *   The file path of the transform.
 */
function si_generate_xml_datastream_with_transform(FedoraObject $o, $transform) {
  $proc = new XSLTProcessor;
  $xsl = new DOMDocument();
  $dom = new DOMDocument();
  $xsl->load($transform);
  $proc->importStyleSheet($xsl); // Attach the xsl rules
  $proc->setParameter('', 'label', $o->label);
  $dom->loadXML('<r></r>'); // The transform generates all the xml.
  $out = $xsl->transformToXML( $dom );
  return $proc->transformToXML($xml);
}