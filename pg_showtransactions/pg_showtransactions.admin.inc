<?php

function pg_showtransactions_edit($form, &$form_state,$t) {

  $form['title'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Title'),
    '#default_value' => $t->title,
  );

  $form['description'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Description'),
    '#default_value' => $t->description,
  );

  $form['email'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Email'),
    '#default_value' => $t->email,
  );

  $form['workflow']=array(
    '#title'         => t('Payment workflow'),
    '#type'          => 'select',
    '#options'       => pgapi_build_workflow(),
    '#default_value' => $t->workflow,
  );

  $form['status']=array(
    '#title'         => t('Payment status'),
    '#type'          => 'select',
    '#options'       => pgapi_build_status(),
    '#default_value' => $t->status,
  );

  $form['gateway']=array(
    '#title'         => t('Payment method'),
    '#type'          => 'select',
    '#options'       => pgapi_get_gateway_options(),
    '#default_value' => $t->gateway,
  );

  $form['txnid']=array(
    '#type'  => 'value',
    '#value' => $t->txnid,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function pg_showtransactions_edit_submit($form, &$form_state) {
  $txnid = $form_state['values']['txnid'];
  $t = pgapi_transaction_load($txnid);
  $t->gateway = $form_state['values']['gateway'];
  $t->status = $form_state['values']['status'];
  $t->workflow = $form_state['values']['workflow'];
  $t->email = $form_state['values']['email'];
  $t->title = $form_state['values']['title'];
  $t->description = $form_state['values']['description'];
  pgapi_transaction_save($t);
}


function pg_showtransactions_history() {

  drupal_add_css(drupal_get_path('module', 'pg_showtransactions') .'/pg_showtransactions.css');


  $header = array(
    'txnid' => array('data' => t('ID'), 'field' => 'pgt.txnid', 'sort' => 'desc'),
    'name' => array('data' => t('User'), 'field' => 'u.name'),
    'email' => array('data' => t('Email'), 'field' => 'pgt.email'),
    'title' => array('data' => t('Title'), 'field' => 'pgt.title'),
    'amount' => array('data' => t('Amount'), 'field' => 'pgt.amount'),
    'gateway' => array('data' => t('Gateway'), 'field' => 'pgt.gateway'),
    'status' => array('data' => t('payment status'), 'field' => 'pgt.status'),
    'created' => array('data' => t('created'), 'field' => 'pgt.created'),
    'changed' => array('data' => t('changed'), 'field' => 'pgt.changed'),
    'operations' => t('Operations')
  );

$query = db_select('pgapi_transaction', 'pgt')->extend('AltPager')->extend('TableSort')->extend('FormFilterAPI');
  $query->fields('pgt');    
  $query->leftjoin('users', 'u', 'pgt.uid = %alias.uid');
  $query->fields('u',array('name'));
  $query->processFilters('pg_showtransactions_history');  
  $query->extend('TableSort');
  $query->orderByHeader($header);
  
  $transactions= $query->execute()->fetchAll();


  foreach ($transactions as $txn) {

    $action = array(
      array( 'href' => "admin/reports/showtransactions/edit/".$txn->txnid,
             'title' => t('Edit'),
             'query' => drupal_get_destination()
           ),
    );           
    $account = user_load($txn->uid);
    $class = array( 'status-'.$txn->status, 'workflow-'.$txn->workflow );    
    
    $rows[] = array(
      'data'=>array(
        $txn->txnid,
        $txn->name ? l($txn->name, "user/".$txn->uid) : variable_get('anonymous',t('anonymous')),
        $txn->email,
        $txn->title,
        array( 'data' => pgapi_format_price($txn->amount), 'class' => array('amount')),
        pgapi_get_gateway_name($txn->gateway),
        pgapi_get_status($txn->status),
        format_date($txn->created,'small'),
        format_date($txn->changed,'small'),
        theme('links',array('links' => $action))
      ),
      'class' => $class,  
    );
  }//while
  if(empty($rows)){
    $rows[] = array(array('data' => t('No data available.'), 'colspan' => '10'));
  }  
  $pagerCountShow = theme('altpager');

  $form = formfilterapi_get_form('pg_showtransactions_history');
  $output = drupal_render($form);
  $output .= '<br>'; 
  $output .= $pagerCountShow;
  $output .= '<div class="pg_showtransaction_history">';
  $output .=  theme('table', array( 'header' => $header, 'rows' => $rows));
  $output .= "</div>";
  $output .= $pagerCountShow;
  
  
  return $output;

}
