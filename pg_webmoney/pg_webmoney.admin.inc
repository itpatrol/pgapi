<?php
/********************************************************************
 * Functions callback
 ********************************************************************/


function pg_webmoney_done_payment (){
  drupal_set_header('Content-type: text/html; charset=iso-8859-1');
  $txnid              = $_POST['LMI_PAYMENT_NO'];
  $purse              = $_POST['LMI_PAYEE_PURSE'];
  $price              = $_POST['LMI_PAYMENT_AMOUNT'];
  $LMI_MODE           = $_POST['LMI_MODE'];
  $LMI_SYS_INVS_NO    = $_POST['LMI_SYS_INVS_NO'];
  $LMI_SYS_TRANS_NO   = $_POST['LMI_SYS_TRANS_NO'];
  $LMI_SYS_TRANS_DATE = $_POST['LMI_SYS_TRANS_DATE'];
  $LMI_SECRET_KEY     = $_POST['LMI_SECRET_KEY'];
  $LMI_PAYER_PURSE    = $_POST['LMI_PAYER_PURSE'];
  $LMI_PAYER_WM       = $_POST['LMI_PAYER_WM'];
  $prerequest         = $_POST['LMI_PREREQUEST'];
  $hash               = $_POST['LMI_HASH'];
  $tocheck = strtoupper( md5($purse . $price . $txnid . $LMI_MODE . $LMI_SYS_INVS_NO . $LMI_SYS_TRANS_NO . $LMI_SYS_TRANS_DATE . $LMI_SECRET_KEY . $LMI_PAYER_PURSE . $LMI_PAYER_WM));

  $t=pgapi_transaction_load($txnid);
  $wmtype='WM'.substr($purse,0,1);
  $stored_price=round($t->extra['prices'][$wmtype],2);

  $res="YES";

  if($stored_price!=$price  ){
    $res = $price.' This number does not match the original price'.$stored_price.' '.$txnid;
    $status = 'failed';
  }
  $wallet=db_fetch_object(db_query('SELECT * FROM {pg_webmoney_rates} WHERE type="%s" ',$wmtype));
  if($wallet->purse != $purse){
    $res = $purse.'Your wallet number does not match the original'.$wallet->purse;
    $status = 'failed';
  }

  if(!empty($hash) ){
    if($hash != $tocheck) {
      $res = t('Security check failed');
      $status = 'denied';
    }
  }

  if($prerequest!=1){
    if($res == "YES"){
      $description = t('Payment from WM %WM and Wallet %purse', array('%WM' => $LMI_PAYER_WM, '%purse' => $LMI_PAYER_PURSE));
      $t->description=$description;
      $t->status = pgapi_get_status_id('completed');
      pgapi_transaction_save($t);
    } else {
      $t->description=$res;
      $t->status = pgapi_get_status_id($status);
      pgapi_transaction_save($t);
    }
  }


echo $res;

}


function pg_webmoney_list() {

  $header = array(
    'purse' => array('data' => t('Purse'), 'field' => 'purse', 'sort' => 'desc'),
    'secretkey' => array('data' => t('Secret Key'), 'field' => 'secret_key'),
    'rate' => array('data' => t('Rate'), 'field' => 'rate'),
    'operations' => array('data' => t('Operations')),
  );

  $query = db_select('pg_webmoney_rates','wr')->extend('PagerDefault')->extend('TableSort');
  $query->fields('wr');
  $query->orderByHeader($header);
  $rates = $query->execute();
  
  $form['#tree'] = TRUE;
  
  foreach($rates as $template){
    $options[$template->type]['purse'] = $template->purse;
    $options[$template->type]['secret_key'] = $template->secret_key;
    $options[$template->type]['rate'] = $template->rate;
    $options[$template->type]['operations'] = theme('links', array (
      'links' => array(
        array(
          'href' => 'admin/config/pgapi/gateway_settings/webmoney/edit/'.$template->type,
          'title' => t('Edit'). ' ',$template->type,
        ),
        array(
          'href' => 'admin/config/pgapi/gateway_settings/webmoney/example/'.$template->purse,
          'title' => t('example'),
        ),
      )));
    
  }//while

  $form['list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No wallet available.'),
  );
  
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Delete checked Wallet'),
  );

  return $form;
}

function pg_webmoney_list_submit($form, &$form_state){
  print_r($form_state['values']['check']);
  foreach($form_state['values']['check'] as $key => $val) {
    if($key === $val){
      db_query("DELETE FROM {pg_webmoney_rates} WHERE type = '%s'", $val);
      $wmtype.=' '.$val;
    }
  }
  drupal_set_message(t('Your wallet types %wmtype has been deleted.',array('%wmtype' => $wmtype)));
  pg_webmoney_cron();
}


function pg_webmoney_edit(&$form_state, $wmtype){
  $pursed = db_fetch_object(db_query('SELECT * FROM {pg_webmoney_rates} WHERE type = "%s" ',$wmtype));
  $form['type'] = array(
    '#type'          => 'hidden',
    '#value'         => $wmtype
  );
  $form['purse'] = array(
    '#type'          => 'markup',
    '#title'         => t('Wallet'),
    '#value' => $pursed->purse,
  );
  $form['secret_key'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Secret key'),
    '#default_value' => $pursed->secret_key,
    '#maxlength'     => 64,
    '#description'   => t("Please enter secret key. If your leave it blank, secret key will not apply."),
//    '#required'      => TRUE,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Update Wallet'),
  );

  return $form;
}

function pg_webmoney_edit_submit($form, &$form_state){

  if(!empty($form_state['values']['type']) ){
    $wmtype=substr($form_state['values']['type'],2,1);
    $rate=pg_webmoney_get_rate($wmtype);
    db_query("UPDATE {pg_webmoney_rates} set  secret_key = '%s' ,rate = '%s' WHERE type='%s'", $form_state['values']['secret_key'],$rate, $form_state['values']['type']);
    drupal_set_message(t('Your wallet type %type has been modified.',array('%type' => $wmtype)));
  }
 
  pg_webmoney_cron();
  $form_state['redirect'] = 'admin/config/pgapi/gateway_settings/webmoney/list';
}


function pg_webmoney_add() {
  $form['purse'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Wallet'),
    '#maxlength'     => 13,
    '#default_value' => '',
    '#description'   => t("Please, enter Wallet number."),
    '#required'      => TRUE,
  );
  $form['rate'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Rate'),
    '#maxlength'     => 13,
    '#default_value' => '',
    '#description'   => t("Please, enter rate to website rate."),
    '#required'      => TRUE,
  );
  $form['secret_key'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Secret key'),
    '#default_value' => '',
    '#maxlength'     => 64,
    '#description'   => t("Please enter secret key. If your leave it blank, secret key will not apply."),
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Add Wallet'),
  );

  return $form;
}

function pg_webmoney_add_validate($form, &$form_state){
  if(!empty($form_state['values']['purse']) ){
    $type=substr($form_state['values']['purse'],0,1);
    $wmtype='WM'.$type;
    $pursed = db_query('SELECT * FROM {pg_webmoney_rates} WHERE type = :type ',array(':type' => $wmtype))->fetchObject();
    if(isset($pursed->type)){
      form_set_error('purse', t('!type already exists.Please enter another wallet number.', array('!type' => $wmtype)));
    }
  }
}

function pg_webmoney_add_submit($form, &$form_state){
  if(!empty($form_state['values']['purse']) ){
    $type=substr($form_state['values']['purse'],0,1);
    $wmtype='WM'.$type;

    $rec = array(
      'type' => $wmtype,
      'purse' => $form_state['values']['purse'],
      'secret_key' => $form_state['values']['secret_key'],
      'rate' => $form_state['values']['rate'],
    );
    drupal_write_record('pg_webmoney_rates',$rec);
    drupal_set_message(t('Your wallet %wallet has been added.',array('%wallet' => $form_state['values']['purse'])));
  }

//  pg_webmoney_cron();
  $form_state['redirect'] = 'admin/config/pgapi/gateway_settings/webmoney/list';

}


function pg_webmoney_payment_end ($type){
  global $user;
  $txnid = $_POST['LMI_PAYMENT_NO'];
  $t=pgapi_transaction_load($txnid);

  $goto = pgapi_get_redirect_url($t);
  drupal_goto($goto);
}



function pg_webmoney_example($purse = NULL){
  $pursed = db_fetch_object(db_query('SELECT * FROM {pg_webmoney_rates} WHERE purse = "%s" ',$purse));
  $output=t('<h1>Merchant purse settings</h1>
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
</form>',array(
     '!purse' => $pursed->purse,
     '!tradename' => variable_get('site_name', 'Drupal'),
     '!secret_key' => $pursed->secret_key,
     '!success' => url('webmoney/success',array('absolute' => TRUE)),
     '!done' => url('webmoney/done',array('absolute' => TRUE)),
     '!fail' => url('webmoney/fail',array('absolute' => TRUE)),
     ));
  return ($output);
}


/********************************************************************
 * Other Functions
 ********************************************************************/


function pg_webmoney_get_rate($type){
  require_once('XML/Unserializer.php');
  $unserializer_options = array (
   'parseAttributes' => TRUE
  );
  $unserializer = &new XML_Unserializer($unserializer_options);
  $result = $unserializer->unserialize(variable_get('pg_webmoney_rate_url',cWebMoneyRateUrl),true);
  $data = $unserializer->getUnserializedData();
  $wmrate='WM'.$type.' - '.variable_get('pg_webmoney_type_wm','WMR');
  if(is_array($data)){
    foreach($data['row'] as $key => $val){
      if($val['Direct'] == $wmrate){
        $pul=substr($val['BaseRate'],0,1);
        if($pul == '-'){
          $rate=1/substr($val['BaseRate'],1);
        }else{
          $rate=$val['BaseRate'];
        }
      }
    }
  }else{
    $rate=1;
  }
  if('WM'.$type==variable_get('pg_webmoney_type_wm','WMR')){
    $rate=variable_get('pg_webmoney_rate_wm','1.00');
  }else{
    $rate= variable_get('pg_webmoney_rate_wm','1.00') / $rate;
  }
  return $rate;
}
