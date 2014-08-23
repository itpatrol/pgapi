<?php
/**
 * @file
 *   Provides theme functions.
 */

/**
 * Theme function for formating price with html.
 *
 * @param array $values
 *   - format Price to format.
 *   - symbol Symbol of currency.
 *   - position Position of price and it symbol.
 *
 * @return string
 *   Formatted price.
 */
function theme_pgapi_format_price($values) {
  if ($values['position']) {
    $output = '<div class="price"><span class="symbol">' . $values['symbol'] . '</span> ' . $values['price'] . '</div>';
  }
  else {
    $output = '<div class="price">' . $values['price'] . '<span class="symbol">' . $values['symbol'] . '</span></div>';
  }

  return $output;
}

/**
 * Theme function for formating price without html.
 *
 * @param array $values
 *   - price Price to format.
 *   - symbol Symbol of currency.
 *   - position Position of price and it symbol.
 *
 * @return string
 *   Formatted price.
 */
function theme_pgapi_format_price_plain($values) {
  if ($values['position']) {
    $output = $values['symbol'] . ' ' . $values['price'];
  }
  else {
    $output = $values['price'] . ' ' . $values['symbol'];
  }
  return $output;
}

/**
 * Implements template_preprocess_THEMENAME().
 */
function template_preprocess_payment(&$values) {
  $transaction = $values['transaction'];
  $values['image'] = drupal_get_path('module', 'pgapi') . '/payment.png';
  $values['service_details'] = module_invoke($transaction->service, 'pgapi_payment_details', $transaction);
}
