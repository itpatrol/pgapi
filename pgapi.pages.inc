<?php

/**
 * @file
 *   Provides page callbacks for menu
 */


/**
 * Page callback for AJAX
 * Loads when user click on radio button on payment/% page
 */
function pgapi_extra_form_load_js($transaction){
  global $user;

  $cached_form_state = array();

  // Load the form from the cache.
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);

  if(!empty($cached_form) && (isset($cached_form['pgapi_fieldset']['pgapi_wrapper']) || isset($cached_form['pgapi_wrapper']))) {

    $form_state = array('values' => $_POST);
    $form = module_invoke($_POST['method'], 'pgapi_gw','get form', $transaction, $form_state);

    if(isset($cached_form['pgapi_wrapper'])) {
      $cached_form['pgapi_wrapper'] = array_merge($cached_form['pgapi_wrapper'], $form);
      form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
    }
    else{
      $cached_form['pgapi_fieldset']['pgapi_wrapper'] = array_merge($cached_form['pgapi_fieldset']['pgapi_wrapper'], $form);
      form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
    }

  }
  else{
    form_set_error('form_token', t('Validation error, please try again. If this error persists, please contact the site administrator.'));
    print drupal_to_js(array('status' => TRUE, 'data' => theme('status_messages')));
    exit();
  }

  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
    '#tree' => TRUE,
    '#parents' => array('pgapi_wrapper'),
  );

  $form_state = array('submitted' => FALSE);
  $form = form_builder('pgapi_extra_form', $form, $form_state);

  $setting = '';
  $javascript = drupal_add_js(NULL, NULL, 'header');
  if(is_array($javascript['setting'])){
    $embed_prefix = "\n<!--//--><![CDATA[//><!--\n";
    $embed_suffix = "\n//--><!]]>\n";
    $setting =
      '<script type="text/javascript">' . $embed_prefix . 'jQuery.extend(Drupal.settings, ' .
      drupal_to_js(call_user_func_array('array_merge_recursive', $javascript['setting'])) .");" . $embed_suffix . "</script>\n";
  }
  $output = $setting . theme('status_messages') . drupal_render($form);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit();
}
