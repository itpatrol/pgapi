<?php
/********************************************************************
 * Functions callback
 ********************************************************************/
function pgapi_pgdata (){
  $item = menu_get_item('admin/pgdata');
  $content = system_admin_menu_block($item);
  $output = theme('admin_block_content', $content);
  return $output;
}

function pgapi_common() {
  $form['#tree'] = TRUE;
  if (module_exists('domain_conf')) { 
    $current_domain = domain_get_domain();
    $pgapi_gw=domain_conf_variable_get($current_domain['domain_id'],'pgapi_gw',array());
  }else{
    $pgapi_gw=variable_get('pgapi_gw',array());
  }
  foreach (module_implements('pgapi_gw') as $module) {
    if ($module_name= module_invoke($module, 'pgapi_gw','display name')) {
      $form[$module]['modulename']=array ('#value' => $module_name);
      $default='';
      if($pgapi_gw[$module]){
        $default='checked';
      }
      $form[$module]['checkbox']=array(
        '#type' => 'checkbox',
        '#default_value' => $default,
        '#domain_setting' => TRUE,
      );
    }
  }

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Update status of payment gateways'),
  );

  return $form;
}

function pgapi_common_submit($form, &$form_state){
  $pgapi_gw=array();
  foreach($form_state['values'] as $key => $val) {
    if($val['checkbox']==1){
      $pgapi_gw[$key]=true;
    }
  }
  if (module_exists('domain_conf')) {
    $current_domain = domain_get_domain();
    $pgapi_gw=domain_conf_variable_set($current_domain['domain_id'],'pgapi_gw',$pgapi_gw);
  }else{
    variable_set('pgapi_gw',$pgapi_gw);
  }
  drupal_set_message(t('Status of payment gateway has been updated.'));
}



/********************************************************************
 * Other Functions
 ********************************************************************/
