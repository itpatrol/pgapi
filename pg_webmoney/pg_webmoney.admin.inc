<?php
/**
 * @file
 * Administer pages for pg_webmoney.module.
 */

function pg_webmoney_done_payment() {
  drupal_add_http_header('Content-type', 'text/html');
  drupal_add_http_header('charset', 'iso-8859-1');

  if (!isset($_POST['LMI_PAYEE_PURSE'])) {
    echo 'ERROR';
    drupal_exit();
  }

  // Load info about webmoney purse.
  $rate = db_select('pg_webmoney_rates', 'r')
    ->fields('r', array('secret_key'))
    ->condition('purse', $_POST['LMI_PAYEE_PURSE'])
    ->execute()
    ->fetchObject();

  if (!$rate) {
    echo 'ERROR';
    drupal_exit();
  }

  $txnid = $_POST['LMI_PAYMENT_NO'];
  $purse = $_POST['LMI_PAYEE_PURSE'];
  $price = $_POST['LMI_PAYMENT_AMOUNT'];
  $lmi_mode = $_POST['LMI_MODE'];
  $lmi_sys_invs_no = $_POST['LMI_SYS_INVS_NO'];
  $lmi_sys_trans_no = $_POST['LMI_SYS_TRANS_NO'];
  $lmi_sys_trans_date = $_POST['LMI_SYS_TRANS_DATE'];
  /**
   * Load secret key from database if in settings merchant not checked
   * "send secret key to result page".
   */
  $lmi_secret_key = isset($_POST['LMI_SECRET_KEY']) ? $_POST['LMI_SECRET_KEY'] : $rate->secret_key;
  $lmi_payer_purse = $_POST['LMI_PAYER_PURSE'];
  $lmi_payer_wm = $_POST['LMI_PAYER_WM'];
  $prerequest = isset($_POST['LMI_PREREQUEST']) ? TRUE : FALSE;
  $hash = $_POST['LMI_HASH'];
  $tocheck = strtoupper(md5($purse . $price . $txnid . $lmi_mode . $lmi_sys_invs_no . $lmi_sys_trans_no . $lmi_sys_trans_date . $lmi_secret_key . $lmi_payer_purse . $lmi_payer_wm));

  $t = pgapi_transaction_load($txnid);
  $wmtype = 'WM' . substr($purse, 0, 1);
  $stored_price = round($t->extra['prices'][$wmtype], 2);

  $res = "YES";

  if ($stored_price != $price) {
    $res = $price . ' This number does not match the original price' . $stored_price . ' ' . $txnid;
    $status = 'failed';
  }

  $wallet = db_select('pg_webmoney_rates', 'r')
    ->fields('r')
    ->condition('type', $wmtype)
    ->execute()
    ->fetchObject();

  if ($wallet->purse != $purse) {
    $res = $purse . 'Your wallet number does not match the original' . $wallet->purse;
    $status = 'failed';
  }

  if (!empty($hash)) {
    if ($hash != $tocheck) {
      $res = t('Security check failed');
      $status = 'denied';
    }
  }

  if ($prerequest != 1) {
    if ($res == 'YES') {
      $description = t('Payment from WM %WM and Wallet %purse', array(
        '%WM' => $lmi_payer_wm,
        '%purse' => $lmi_payer_purse,
      ));
      $t->description = $description;
      $t->status = pgapi_get_status_id('completed');
      pgapi_transaction_save($t);
    }
    else {
      $t->description = $res;
      $t->status = pgapi_get_status_id($status);
      pgapi_transaction_save($t);
    }
  }

  echo $res;
}

function pg_webmoney_list() {
  $header = array(
    'purse' => array(
      'data' => t('Purse'),
      'field' => 'purse',
      'sort' => 'desc',
    ),
    'secret_key' => array('data' => t('Secret Key'), 'field' => 'secret_key'),
    'rate' => array('data' => t('Rate'), 'field' => 'rate'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('pg_webmoney_rates', 'wr')
    ->extend('PagerDefault')
    ->extend('TableSort');

  $query->fields('wr');
  $query->orderByHeader($header);
  $rates = $query->execute();

  $form['#tree'] = TRUE;

  $options = array();

  foreach ($rates as $template) {
    $options[$template->type]['purse'] = $template->purse;
    $options[$template->type]['secret_key'] = $template->secret_key;
    $options[$template->type]['rate'] = $template->rate;
    $options[$template->type]['operations'] = theme('links', array(
      'links' => array(
        array(
          'href' => 'admin/config/pgapi/gateway_settings/webmoney/edit/' . $template->type,
          'title' => t('Edit') . ' ',
          $template->type,
        ),
        array(
          'href' => 'admin/config/pgapi/gateway_settings/webmoney/example/' . $template->purse,
          'title' => t('example'),
        ),
      ),
    ));
  }

  $form['list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No wallet available.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete checked Wallet'),
  );

  return $form;
}

function pg_webmoney_list_submit($form, &$form_state) {
  // @fixme print_r($form_state['values']['check']);
  $wmtype = '';

  foreach ($form_state['values']['check'] as $key => $val) {
    if ($key === $val) {
      db_delete('pg_webmoney_rates')
        ->condition('type', $val)
        ->execute();
      $wmtype .= ' ' . $val;
    }
  }

  drupal_set_message(t('Your wallet types %wmtype has been deleted.', array(
    '%wmtype' => $wmtype,
  )));
//  pg_webmoney_cron();
}


function pg_webmoney_edit($form, &$form_state, $wmtype) {
  $pursed = db_select('pg_webmoney_rates', 'r')
    ->fields('r')
    ->condition('type', $wmtype)
    ->execute()
    ->fetchObject();

  $form['type_origin'] = array(
    '#type' => 'hidden',
    '#value' => $wmtype,
  );

  $form['type'] = array(
    '#type' => 'item',
    '#title' => t('Type'),
    '#markup' => $wmtype,
  );

  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Wallet'),
    '#default_value' => $pursed->purse,
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Rate'),
    '#default_value' => $pursed->rate,
    '#required' => TRUE,
  );

  $form['secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret key'),
    '#default_value' => $pursed->secret_key,
    '#maxlength' => 64,
    '#description' => t('Please enter secret key. If your leave it blank, secret key will not apply.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Wallet'),
    '#validate' => array('pg_webmoney_edit_validate'),
  );

  return $form;
}

function pg_webmoney_edit_submit($form, &$form_state) {
  $values = $form_state['values'];
  $type = substr($values['purse'], 0, 1);
  // Real purse type.
  $wm_type = 'WM' . $type;

  try {
    db_update('pg_webmoney_rates')
      ->fields(array(
        'type' => $wm_type,
        'purse' => $values['purse'],
        'rate' => $values['rate'],
        'secret_key' => $values['secret_key'],
      ))
      ->condition('type', $values['type_origin'])
      ->execute();

    drupal_set_message(t('Your wallet type %type has been modified.', array(
      '%type' => $wm_type,
    )));
  } catch (\PDOException $e) {
    drupal_set_message(t('Data purse were not updated.'));
  }

  $form_state['redirect'] = 'admin/config/pgapi/gateway_settings/webmoney/list';
}

function pg_webmoney_edit_validate($form, &$form_state) {
  $values = $form_state['values'];

  if (!empty($values['purse'])) {
    $type = substr($values['purse'], 0, 1);
    $wm_type = 'WM' . $type;
    $pursed = db_query('SELECT * FROM {pg_webmoney_rates} WHERE type = :type ', array(':type' => $wm_type))->fetchObject();

    if (isset($pursed->type) && $wm_type !== $values['type_origin']) {
      form_set_error('purse', t('!type already exists.Please enter another wallet number.', array('!type' => $wm_type)));
    }
  }
}

function pg_webmoney_add() {
  $form['purse'] = array(
    '#type' => 'textfield',
    '#title' => t('Wallet'),
    '#maxlength' => 13,
    '#default_value' => '',
    '#description' => t('Please, enter Wallet number.'),
    '#required' => TRUE,
  );

  $form['rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Rate'),
    '#maxlength' => 13,
    '#default_value' => '',
    '#description' => t('Please, enter rate to website rate.'),
    '#required' => TRUE,
  );

  $form['secret_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret key'),
    '#default_value' => '',
    '#maxlength' => 64,
    '#description' => t('Please enter secret key. If your leave it blank, secret key will not apply.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add Wallet'),
  );

  return $form;
}

function pg_webmoney_add_validate($form, &$form_state) {
  if (!empty($form_state['values']['purse'])) {
    $type = substr($form_state['values']['purse'], 0, 1);
    $wmtype = 'WM' . $type;

    $pursed = db_select('pg_webmoney_rates', 'r')
      ->fields('r')
      ->condition('type', $wmtype)
      ->execute()
      ->fetchObject();

    if (isset($pursed->type)) {
      form_set_error('purse', t('!type already exists.Please enter another wallet number.', array('!type' => $wmtype)));
    }
  }
}

function pg_webmoney_add_submit($form, &$form_state) {
  if (!empty($form_state['values']['purse'])) {
    $type = substr($form_state['values']['purse'], 0, 1);
    $wmtype = 'WM' . $type;

    $rec = array(
      'type' => $wmtype,
      'purse' => $form_state['values']['purse'],
      'secret_key' => $form_state['values']['secret_key'],
      'rate' => $form_state['values']['rate'],
    );
    drupal_write_record('pg_webmoney_rates', $rec);

    drupal_set_message(t('Your wallet %wallet has been added.', array(
      '%wallet' => $form_state['values']['purse'],
    )));
  }

  // @fixme pg_webmoney_cron();
  $form_state['redirect'] = 'admin/config/pgapi/gateway_settings/webmoney/list';
}

function pg_webmoney_example($purse = NULL) {
  $pursed = db_select('pg_webmoney_rates', 'r')
    ->fields('r')
    ->condition('purse', $purse)
    ->execute()
    ->fetchObject();

  $output = t('<h1>Merchant purse settings</h1>
<table>
<tbody><tr>
<td nowrap="nowrap">Purse:</td>

<td class="header3" align="left"><b>!purse</b></td>
<td>&nbsp;</td>
<td align="left">&nbsp;</td>
</tr>
<tr>
<td nowrap="nowrap">Trade Name:</td>
<td align="left"><input style="display: inline;" value="!tradename" size="50" id="m_name" name="m_name" type="text"></td>
<td align="center">&nbsp;-&nbsp;</td>
<td align="left"> is displayed on the web page during the payment </td>
</tr>
<tr>
<td nowrap="nowrap">Secret Key:</td>

<td align="left"><input style="display: inline;" value="!secret_key" id="secret_key" name="secret_key" size="50" type="text"></td>
<td align="center">
<input checked="checked" id="send_secret_key" name="send_secret_key" type="checkbox"></td>
<td align="left">Send the Secret Key to the Result URL if the Result URL is secured</td>
</tr>
<tr>
<td nowrap="nowrap">Result URL:</td>
<td align="left"><input style="display: inline;" value="!done" id="result_url" name="result_url" size="50" maxlength="255" type="text"></td>
<td align="center">
<input checked="checked" id="send_param_prerequest" name="send_param_prerequest" value="1" type="checkbox">
</td>
<td align="left">
Send parameters in the pre-request
</td>
</tr>

<tr>
<td nowrap="nowrap">Success URL:</td>
<td align="left"><input style="display: inline;" value="!success" id="success_url" name="success_url" size="50" maxlength="255" type="text"></td>
<td align="center">
<select name="success_method" id="success_method">
<option value="0">GET</option>
<option value="1" selected="selected" >POST</option>
<option value="2">LINK</option>
</select>
</td>
<td>
method of requesting Success URL
</td>

</tr>
<tr>
<td nowrap="nowrap">Fail URL:</td>
<td align="left"><input style="display: inline;" value="!fail" id="fail_url" name="fail_url" size="50" maxlength="255" type="text"></td>
<td align="center">
<select name="fail_method" id="fail_method">
<option value="0">GET</option>
<option value="1" selected="selected">POST</option>
<option value="2">LINK</option>
</select>
</td>
<td>
method of requesting Fail URL

</td>
</tr>
</tbody></table>
<table>
<tbody><tr>
<td nowrap="nowrap">
Allow overriding URL from Payment Request Form:
</td>
<td align="left">
<input id="allow_url_from_form" name="allow_url_from_form" value="1" type="checkbox">
</td>
<td></td>
<td></td>
</tr>
<tr>
<td nowrap="nowrap">
Send an error notification to merchant\'s keeper:

</td>
<td align="left">
<input checked="checked" id="send_error" name="send_error" value="1" type="checkbox">
</td>
<td></td>
<td></td>
</tr>
<tr>
<td nowrap="nowrap">Signature algorithm:</td>
<td align="left">
<select name="auth_type" id="auth_type">
<option value="0">SIGN</option>
<option value="1" selected="selected">MD5</option>
</select>

</td>
<td></td>
<td></td>
</tr>





</tbody></table>
</form>', array(
    '!purse' => $pursed->purse,
    '!tradename' => variable_get('site_name', 'Drupal'),
    '!secret_key' => $pursed->secret_key,
    '!success' => url('payment/success/webmoney', array('absolute' => TRUE)),
    '!done' => url('webmoney/done', array('absolute' => TRUE)),
    '!fail' => url('payment/fail/webmoney', array('absolute' => TRUE)),
  ));

  return ($output);
}
