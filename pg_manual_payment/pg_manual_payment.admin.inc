<?php
/**
 * @file
 * Administer pages for pg_manual_payment.module.
 */

/**
 * Implements hook_FORMID().
 */
function pg_manual_payment_list() {
  $accounts = db_select('pg_manual_payment_accounts', 'a')
    ->fields('a')
    ->orderBy('name')
    ->execute()
    ->fetchAll();
  $form['#tree'] = TRUE;

  foreach ($accounts as $account) {
    $edit_url = 'admin/pgdata/pgsettings/manual_payment/edit/' . $account->paid;
    $edit_link = l($account->name, $edit_url, array('query' => drupal_get_destination()));

    $form[$account->paid]['purse'] = array('#value' => $account->purse);
    $form[$account->paid]['rate'] = array('#value' => $account->rate);
    $form[$account->paid]['edit'] = $edit_link;
    $purses[$account->paid] = '';
  }

  $form['check'] = array(
    '#type' => 'checkboxes',
    '#options' => $purses,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete checked Accounts'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_manual_payment_list_submit($form, &$form_state) {
  foreach ($form_state['values']['check'] as $key => $val) {
    if ($key === $val) {
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
function pg_manual_payment_edit(&$form_state, $paid) {
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
    '#description' => t("Please, enter Payment system name."),
    '#required' => TRUE,
  );

  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => $pursed->purse,
    '#description' => t("Please, enter Payment system account number."),
    '#required' => TRUE,
  );

  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency Symbol'),
    '#default_value' => $pursed->symbol,
    '#description' => t("Please, enter Payment system Currency symbol."),
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency rate'),
    '#default_value' => $pursed->rate,
    '#description' => t("Please, enter Payment system Currency rate."),
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
  drupal_set_message(t('Your Account %name has been modified.', array('%name' => $form_state['values']['name'])));
}

/**
 * Implements hook_FORMID().
 */
function pg_manual_payment_add() {
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => '',
    '#description' => t("Please, enter Payment system name."),
    '#required' => TRUE,
  );

  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
    '#default_value' => '',
    '#description' => t("Please, enter Payment system account number."),
    '#required' => TRUE,
  );

  $form['symbol'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency Symbol'),
    '#default_value' => '',
    '#description' => t("Please, enter Payment system Currency symbol."),
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency rate'),
    '#default_value' => '',
    '#description' => t("Please, enter Payment system Currency rate."),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Account'),
  );

  return $form;
}

/**
 * Implements hook_FORMID_submit().
 */
function pg_manual_payment_add_submit($form, &$form_state) {
  drupal_write_record('pg_manual_payment_accounts', $form_state['values']);
  drupal_set_message(t('Your Account %name has been added.', array('%name' => $form_state['values']['name'])));
}

/**
 * Page settings module.
 */
function pg_manual_payment_settings() {
  $form['pg_manual_payment_email'] = array(
    '#type' => 'textfield',
    '#title' => t("Admin email with notification emails"),
    '#default_value' => variable_get('pg_manual_payment_email', ''),
    '#description' => t("Please enter email address for receive a notify and warning messages."),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}
