<dl class="payment">

 <dt><?php print t('Amount')?></dt>
 <dd><?php print pgapi_format_price($transaction->amount) ?></dd>

 <?php if (!empty($transaction->description)) : ?>
 <dt><?php print t('Description')?></dt>
 <dd><?php print $transaction->description ?></dd>
 <?php endif ?>

 <dt><?php print t('Date')?></dt>
 <dd><?php print format_date($transaction->changed, 'custom', ' d M Y H:i:s') ?></dd>

 <dt><?php print t('Status')?></dt>
 <dd class="status-<?php print $transaction->status ?>"><?php print pgapi_get_status($transaction->status) ?></dd>

 <?php if (!empty($transaction->gateway)) : ?>
 <dt><?php print t('Paid with')?></dt>
 <dd><?php
   print module_invoke($transaction->gateway, 'pgapi_gw', 'display name') ?></dd>
 <?php endif ?>
<?php
  if(is_array($service_details)){
    foreach($service_details as $title => $value){
      echo  '<dt>' . $title . '</dt>'."\n";
      echo  '<dd>' . $value . '</dd>'."\n";
    }
  }
?>

</dl>
