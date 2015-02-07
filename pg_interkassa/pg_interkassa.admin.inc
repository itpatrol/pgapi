<?php
/**
 * @file
 * Administer pages for pg_interkassa.module.
 */

/**
 * Finished payment.
 */
function pg_interkassa_done_payment() {
  $tx_nid = check_plain($_POST['ik_pm_no']);
  $ik_co_id = check_plain($_POST['ik_co_id']);
  $ik_am = check_plain($_POST['ik_am']);
  $ik_cur = check_plain($_POST['ik_cur']);
  $ik_sign = check_plain($_POST['ik_sign']);

  $description = t('Payment has been received.');
  $status = 'completed';

  $data_set = $_POST;

  $secret_key = variable_get('pg_interkassa_secret_key');

  unset($data_set['ik_sign']);
  ksort($data_set, SORT_STRING);
  array_push($data_set, $secret_key);
  $sign_string = implode(':', $data_set);
  $sign = base64_encode(md5($sign_string, TRUE));

  if ($sign !== $ik_sign) {
    $status = 'denied';
  }

  $co_id = variable_get('pg_interkassa_co_id');

  if ($co_id != $ik_co_id) {
    $status = 'denied';
  }

  $t = pgapi_transaction_load($tx_nid);

  if ($t->extra['amount'] != $ik_am) {
    $description = t('This amount does not match with the original price');
    $status = 'denied';
  }

  $currency = variable_get('pg_interkassa_currency_code', 'USD');

  if ($currency != $ik_cur) {
    $description = t('This currency does not match with the original currency');
    $status = 'denied';
  }

  if ($status != 'completed') {
    drupal_add_http_header('Status', t('400 Bad Request: Invalid request parameters.'));
  }

  $t->description = $description;
  $t->status = pgapi_get_status_id($status);
  pgapi_transaction_save($t);
}

/**
 * Setting page.
 */
function pg_interkassa_settings($form, $form_state) {
  $form['pg_interkassa_co_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Checkout id'),
    '#default_value' => variable_get('pg_interkassa_co_id'),
    '#description' => t('Please enter Interkassa checkout id.'),
    '#required' => TRUE,
  );

  $form['pg_interkassa_interaction_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Interaction URL '),
    '#default_value' => variable_get('pg_interkassa_interaction_url', PG_INTERKASSA_PAY_INTERACTION_URL),
    '#description' => t("Please enter interaction url.\nDefault: !url", array('!url' => PG_INTERKASSA_PAY_INTERACTION_URL)),
  );

  $currency_codes = array(
    'USD' => 'USD - U.S. Dollars',
    'EUR' => 'EUR - Euros',
    'UAH' => 'UAH - Ukrainian Hryvnia',
    'RUB' => 'RUB - Russian Ruble',
  );

  $form['pg_interkassa_currency_code'] = array(
    '#type' => 'select',
    '#title' => t('Currency code'),
    '#options' => $currency_codes,
    '#default_value' => variable_get('pg_interkassa_currency_code', 'USD'),
    '#description' => t("The currency code that Interkassa should process the payment in."),
  );

  $form['pg_interkassa_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Rate to site currency'),
    '#default_value' => variable_get('pg_interkassa_rate', '1.00'),
    '#description' => t('Please enter Interkassa rate according to site currency.'),
  );

  $form['pg_interkassa_secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret key'),
    '#default_value' => variable_get('pg_interkassa_secret_key'),
    '#description' => t('Please enter secret key. If your leave it blank, secret key will not apply.'),
    '#required' => TRUE,
  );

  $form['#validate'][] = 'pg_interkassa_settings_validate';

  return system_settings_form($form);
}

/**
 * Implements hook_FORMID_validate().
 */
function pg_interkassa_settings_validate($form, &$form_state) {
  if ($form_state['values']['pg_interkassa_rate'] <= 0) {
    form_set_error('pg_interkassa_rate', t('%rate must be more 0.', array('%rate' => $form_state['values']['pg_interkassa_rate'])));
  }
}
