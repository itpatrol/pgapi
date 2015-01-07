<?php

function pg_showtransactions_personal_history($account) {
  drupal_add_css(drupal_get_path('module', 'pg_showtransactions') . '/pg_showtransactions.css');

  $header = array(
    'txnid' => array('data' => t('ID'), 'field' => 'pgt.txnid', 'sort' => 'desc'),
    'title' => array('data' => t('Title'), 'field' => 'pgt.title'),
    'amount' => array('data' => t('Amount'), 'field' => 'pgt.amount'),
    'gateway' => array('data' => t('Gateway'), 'field' => 'pgt.gateway'),
    'status' => array('data' => t('payment status'), 'field' => 'pgt.status'),
    'created' => array('data' => t('created'), 'field' => 'pgt.created'),
    'changed' => array('data' => t('changed'), 'field' => 'pgt.changed'),
    'operations' => t('Operations'),
  );

  $query = db_select('pgapi_transaction', 'pgt')
    ->extend('AltPager')
    ->extend('TableSort')
    ->extend('FormFilterAPI');

  $query->fields('pgt');
  $query->condition('pgt.uid', $account->uid);
  $query->processFilters('pg_showtransactions_personal_history');  
  $query->extend('TableSort');
  $query->orderByHeader($header);

  $transactions= $query->execute()->fetchAll();
  foreach ($transactions as $txn) {
    $class = array( 'status-' . $txn->status, 'workflow-' . $txn->workflow );

    $action = '';
    if ($txn->status == PG_PENDING) {
      $action = t('Unpaid ') . ' ' . l(t('Pay now'), 'payment/' . $txn->txnid, array('query' => drupal_get_destination(), 'fragment' => 'pgapi-payment'));
    }

    $rows[] = array(
      'data' => array(
        $txn->txnid,
        $txn->title,
        array('data' => pgapi_format_price($txn->amount), 'class' => array('amount')),
        pgapi_get_gateway_name($txn->gateway),
        pgapi_get_status($txn->status),
        array('data' => format_date($txn->created, 'custom', 'd M Y'), 'class' => 'date'),
        array('data' => format_date($txn->changed, 'custom', 'd M Y'), 'class' => 'date'),
        array('data' => $action, 'class' => 'status'),
      ),
      'class' => $class,
    );
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No data available.'), 'colspan' => 8));
  }

  $pager_count_show = theme('altpager');

  $form = formfilterapi_get_form('pg_showtransactions_personal_history');
  $output = drupal_render($form);
  $output .= '<br />';
  $output .= $pager_count_show;
  $output .= '<div class="pg_showtransaction_history">';
  $output .=  theme('table', array('header' => $header, 'rows' => $rows));
  $output .= '</div>';
  $output .= $pager_count_show;

  return $output;
}
