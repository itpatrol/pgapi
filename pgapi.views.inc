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
    'txnid' => array(
      'title' => 'Txnid',
      'help' => t('Primary Key: Unique transaction ID.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'uid' => array(
      'title' => t('Uid'),
      'help' => t('The users.uid of the user who created transaction.'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => FALSE,
      ),
      'relationship' => array(
        'base' => 'users',
        'base field' => 'uid',
        'handler' => 'views_handler_relationship',
        'label' => t('user'),
        'title' => t('User'),
        'help' => t('The users.uid of the user who created transaction.'),
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
  );

  return $data;
}
