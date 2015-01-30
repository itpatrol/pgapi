<?php
/**
 * @file
 * Admin settings pg_2co.module.
 */

/**
 * Page with settings module.
 */
function pg_2co_settings($form, &$form_state) {
  $form['pg_2co_action_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Action url'),
    '#default_value' => variable_get('pg_2co_action_url', PG_2CO_ACTION_URL_SINGLE),
    '#description' => t("Please enter action url.\nDefault: !url", array('!url' => PG_2CO_ACTION_URL_SINGLE)),
  );

  $form['pg_2co_sid'] = array(
    '#type' => 'textfield',
    '#title' => t('Vendor account number'),
    '#description' => t('Your 2Checkout vendor account number.'),
    '#size' => 16,
    '#default_value' => variable_get('pg_2co_sid', ''),
  );

  $form['pg_2co_secret_word'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret word for order verification'),
    '#description' => t('The secret word entered in your 2Checkout account Look and Feel settings.'),
    '#size' => 16,
    '#default_value' => variable_get('pg_2co_secret_word', 'tango'),
  );

  $form['pg_2co_demo'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable demo mode, allowing you to process fake orders for testing purposes.'),
    '#default_value' => variable_get('pg_2co_demo', TRUE),
  );

  $form['pg_2co_language'] = array(
    '#type' => 'select',
    '#title' => t('Language preference'),
    '#description' => t('Adjust language on 2Checkout pages.'),
    '#options' => array(
      'en' => t('English'),
      'sp' => t('Spanish'),
    ),
    '#default_value' => variable_get('pg_2co_language', 'en'),
  );

  $form['pg_2co_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Rate to site currency'),
    '#default_value' => variable_get('pg_2co_rate', '1.00'),
    '#description' => t("Please enter 2co rate according to site currency."),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_validate().
 */
function pg_2co_settings_validate($form, &$form_state) {
  if ($form_state['values']['pg_2co_rate'] <= 0) {
    form_set_error('pg_2co_rate', t('%rate must be more 0.', array('%rate' => $form_state['values']['pg_2co_rate'])));
  }
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_2co_settings_submit($form, &$form_state) {
  if (isset($form_state['values']['pg_2co_action_url'])) {
    variable_set('pg_2co_action_url', $form_state['values']['pg_2co_action_url']);
  }

  if (isset($form_state['values']['pg_2co_sid'])) {
    variable_set('pg_2co_sid', $form_state['values']['pg_2co_sid']);
  }

  if (isset($form_state['values']['pg_2co_secret_word'])) {
    variable_set('pg_2co_secret_word', $form_state['values']['pg_2co_secret_word']);
  }

  if (isset($form_state['values']['pg_2co_demo'])) {
    variable_set('pg_2co_demo', $form_state['values']['pg_2co_demo']);
  }

  if (isset($form_state['values']['pg_2co_language'])) {
    variable_set('pg_2co_language', $form_state['values']['pg_2co_language']);
  }

  if (isset($form_state['values']['pg_2co_check'])) {
    variable_set('pg_2co_check', $form_state['values']['pg_2co_check']);
  }

  if (isset($form_state['values']['pg_2co_rate'])) {
    variable_set('pg_2co_rate', $form_state['values']['pg_2co_rate']);
  }
}

/**
 * Processing a new payment.
 */
function pg_2co_done_payment() {
  if (isset($_REQUEST['key'])) {
    $tx_nid = $_REQUEST['merchant_order_id'];
    $payment_amount = check_plain($_REQUEST['total']);
    $payment_currency = check_plain($_REQUEST['currency_code']);

    $hash_secret_word = variable_get('pg_2co_secret_word', '');
    $hash_sid = variable_get('pg_2co_sid', '');
    $hash_order = $_REQUEST['order_number'];

    if (variable_get('pg_2co_demo', TRUE)) {
      $hash_order = 1;
    }

    $string_to_hash = strtoupper(md5($hash_secret_word . $hash_sid . $hash_order . $payment_amount));

    $description = t('Payment has been received.');
    $status = 'completed';

    if ($string_to_hash != $_REQUEST['key']) {
      $status = 'denied';
    }

    $t = pgapi_transaction_load($tx_nid);

    if (number_format($t->extra['amount'], 2) != number_format($payment_amount, 2)) {
      $description = t('This amount does not match with the original price');
      $status = 'denied';
    }

    $currency = variable_get('pg_2co_currency_code', 'USD');

    if ($currency != $payment_currency) {
      $description = t('This currency does not match with the original currency');
      $status = 'denied';
    }

    $t->description = $description;
    $t->status = pgapi_get_status_id($status);
    pgapi_transaction_save($t);
  }
}
