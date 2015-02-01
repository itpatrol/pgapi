<?php
/**
 * @file
 * Provide views data for pgapi.module.
 */

/**
 * Implements of hook_views_data().
 */
function pgapi_views_data() {
  $data['pgapi_transaction'] = array(
    'table' => array(
      'base' => array(
        'field' => 'txnid',
        'title' => t('Transactions'),
        'help' => 'Transaction information.',
      ),
      'group' => t('PGAPI'),
    ),
  );

  return $data;
}
