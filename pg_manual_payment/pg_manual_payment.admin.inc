<?php
/**
 * @file
 * Administer pages for pg_manual_payment.module.
 */

/**
 * Implements hook_FORMID().
 */
function pg_manual_payment_list($form, &$form_state) {
  $header = array(
    '',
    t('Account name'),
    t('Account number'),
    t('Rate'),
    t('Symbol'),
  );

  $accounts = db_select('pg_manual_payment_accounts', 'a')
    ->fields('a')
    ->orderBy('name')
    ->execute()
    ->fetchAll();

  $options = array();
  foreach ($accounts as $account) {
    $edit_url = 'admin/config/pgapi/gateway_settings/manual_payment/edit/' . $account->paid;
    $edit_link = l($account->name, $edit_url, array('query' => drupal_get_destination()));

    $options[$account->paid] = array(
      $edit_link,
      $account->name,
      $account->purse,
      $account->rate,
      $account->symbol,
    );
  }

  $form['check'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete checked accounts'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_manual_payment_list_submit($form, &$form_state) {
  foreach ($form_state['values']['check'] as $key => $val) {
    if ($key == $val) {
      $pursed = db_select('pg_manual_payment_accounts', 'a')
        ->fields('a')
        ->condition('paid', $key)
        ->execute()
        ->fetchObject();

      db_delete('pg_manual_payment_accounts')
        ->condition('paid', $val)
        ->execute();

      drupal_set_message(t('Your account %name has been deleted.', array('%name' => $pursed->name)));
    }
  }
}

/**
 * Implements hook_FORMID().
 */
function pg_manual_payment_edit($form, &$form_state, $paid) {
  $pursed = db_select('pg_manual_payment_accounts', 'a')
    ->fields('a')
    ->condition('paid', $paid)
    ->execute()
    ->fetchObject();

  $form['paid'] = array(
    '#type' => 'value',
    '#value' => $paid,
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $pursed->name,
    '#description' => t("Please, enter payment system name."),
    '#required' => TRUE,
  );

  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Account number'),
    '#default_value' => $pursed->purse,
    '#description' => t("Please, enter Payment system account number."),
    '#required' => TRUE,
  );

  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency Symbol'),
    '#default_value' => $pursed->symbol,
    '#description' => t("Please, enter payment system currency symbol."),
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency rate'),
    '#default_value' => $pursed->rate,
    '#description' => t("Please, enter payment system currency rate."),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_manual_payment_edit_submit($form, &$form_state) {
  drupal_write_record('pg_manual_payment_accounts', $form_state['values'], 'paid');
  drupal_set_message(t('Your account %name has been modified.', array('%name' => $form_state['values']['name'])));
}

/**
 * Implements hook_FORMID().
 */
function pg_manual_payment_add() {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => '',
    '#description' => t('Please, enter payment system name.'),
    '#required' => TRUE,
  );

  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => '',
    '#description' => t('Please, enter payment system account number.'),
    '#required' => TRUE,
  );

  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency symbol'),
    '#default_value' => '',
    '#description' => t('Please, enter payment system currency symbol.'),
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency rate'),
    '#default_value' => '',
    '#description' => t('Please, enter payment system currency rate.'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add account'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_manual_payment_add_submit($form, &$form_state) {
  drupal_write_record('pg_manual_payment_accounts', $form_state['values']);
  drupal_set_message(t('Your account %name has been added.', array('%name' => $form_state['values']['name'])));
}

/**
 * Page settings module.
 */
function pg_manual_payment_settings() {
  $form['pg_manual_payment_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Admin email with notification emails'),
    '#default_value' => variable_get('pg_manual_payment_email', ''),
    '#description' => t('Please enter email address for receive a notify and warning messages.'),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
