<?php

/**
 * @file
 * Administative functions for PGAPI module
 */

/**
 * Menu callback to list sub-items.
 */
function pgapi_admin_menu_block_page() {
  $item = menu_get_item();
  module_load_include('inc', 'system', 'system.admin');
  if ($content = system_admin_menu_block($item)) {
    $output = theme('admin_block_content', array('content' => $content));
  }
  else {
    $output = t('You do not have any administrative items.');
  }
  return $output;
}

/**
 * Form-builder for PGAPI currency settings.
 */

function pgapi_admin_currency_settings($form, &$form_state) {

  $pgapi_currency = variable_get('pgapi_currency', array());
  $form['currency']=array(
    '#tree' => TRUE,
  );
  
  $form['currency']['symbol'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Symbol'),
    '#default_value' => isset($pgapi_currency['symbol']) ? $pgapi_currency['symbol'] : '' ,
    '#description'   => t("Please enter symbol of the Account Balance currency."),
    '#required'      => true,
  );
  
  
  $form['currency']['decimal_places'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Decimal places'),
    '#default_value' => isset($pgapi_currency['decimal_places']) ? $pgapi_currency['decimal_places'] : '',
    '#description'   => t("Please enter number of decimal places."),
    '#required'      => true,
  );
  
  $form['currency']['decimal'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Decimal delimiter'),
    '#default_value' => isset($pgapi_currency['decimal']) ? $pgapi_currency['decimal'] : '',
    '#description'   => t("Please enter decimal delimiter."),
    '#required'      => true,
  );
  
  $form['currency']['thousands'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Thousands places'),
    '#default_value' => isset($pgapi_currency['thousands']) ? $pgapi_currency['thousands'] : '',
    '#description'   => t("Please enter thousands delimeter."),
    '#required'      => true,
  );
  
  $form['currency']['symbol_position'] = array(
    '#type'          => 'radios',
    '#title'         => t('Simbol position'),
    '#options'       => array ( 1 => t('Left'), 0 => t('Right')),
    '#default_value' => isset($pgapi_currency['symbol_position']) ? $pgapi_currency['symbol_position'] : 0,
    '#description'   => t("Please select symbol position."),
    '#required'      => true,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save currency settings'),
  );
  
  return $form;
}

/**
 * Submit function for PGAPI common settings form.
 */
function pgapi_admin_currency_settings_submit($form, &$form_state) {
  $pgapi_currency = array_filter($form_state['values']['currency']);

  // Save enabled gateways.
  variable_set('pgapi_currency', $pgapi_currency);

  drupal_set_message(t('Currency settings saved.'));
}

/**
 * Form-builder for PGAPI gateway settings.
 */
function pgapi_admin_gateway_settings($form, &$form_state) {

  $pgapi_gw = pgapi_get_enabled_payment_systems();
  $gateways = pgapi_get_gateway_options();

  if (empty($gateways)) {
    $form['gateways']['#markup'] = t('No payment gateways available.');
  }
  else {
    $form['gateways'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Payment gateways'),
      '#options' => $gateways,
      '#default_value' => $pgapi_gw,
    );
    // Add the buttons.
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update status of payment gateways'),
    );
  }

  return $form;
}

/**
 * Submit function for PGAPI common settings form.
 */
function pgapi_admin_gateway_settings_submit($form, &$form_state) {
  $pgapi_gw = array_filter($form_state['values']['gateways']);

  // Save enabled gateways.
  variable_set('pgapi_gw', $pgapi_gw);

  drupal_set_message(t('Status of payment gateway has been updated.'));
}

