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
    'email' => array(
      'title' => t('Email'),
      'help' => t('User email.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => FALSE,
      ),
    ),
    'title' => array(
      'title' => t('Title'),
      'help' => t('Transaction title.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => FALSE,
      ),
    ),
    'amount' => array(
      'title' => t('Amount'),
      'help' => t('Transaction amount.'),
      'field' => array(
        'handler' => 'pgapi_views_handler_field_amount',
        'click sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_field',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'gateway' => array(
      'title' => t('Gateway'),
      'help' => t('Transaction gateway.'),
      'field' => array(
        'handler' => 'pgapi_views_handler_field_gateway',
        'click sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_field',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'status' => array(
      'title' => t('Status'),
      'help' => t('Transaction status.'),
      'field' => array(
        'handler' => 'pgapi_views_handler_field_status',
        'click sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_field_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'created' => array(
      'title' => t('Created'),
      'help' => t('Transaction created date.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
    ),
    'changed' => array(
      'title' => t('Changed'),
      'help' => t('Transaction changed date.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => FALSE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
    ),
    'edit_link' => array(
      'title' => t('Edit link'),
      'help' => t('Transaction link edit.'),
      'field' => array(
        'handler' => 'pgapi_views_handler_field_edit_link',
      ),
    ),
  );

  return $data;
}

/**
 * Implements hook_views_handlers()
 */
function pgapi_views_handlers() {
  return array(
    'handlers' => array(
      'pgapi_views_handler_field_amount' => array(
        'parent' => 'views_handler_field',
        'file' => 'pgapi_views_handler_field_amount.inc',
      ),
      'pgapi_views_handler_field_gateway' => array(
        'parent' => 'views_handler_field',
        'file' => 'pgapi_views_handler_field_gateway.inc',
      ),
      'pgapi_views_handler_field_status' => array(
        'parent' => 'views_handler_field',
        'file' => 'pgapi_views_handler_field_status.inc',
      ),
      'pgapi_views_handler_field_edit_link' => array(
        'parent' => 'views_handler_field_url',
        'file' => 'pgapi_views_handler_field_edit_link.inc',
      ),
    ),
  );
}
