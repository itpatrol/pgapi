<?php

/**
 * @file
 *   Provides theme functions
 */

/**
 * Theme function for formating price with html
 *
 * @param  $price
 *   Price to format
 * @param  $symbol
 *   Symbol of currency
 * @param bool $position
 *   Position of price and it symbol
 * @return string
 *   Formated price
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
 * Theme function for formating price without html
 *
 * @param  $price
 *   Price to format
 * @param  $symbol
 *   Symbol of currency
 * @param bool $position
 *   Position of price and it symbol
 * @return string
 *   Formated price
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
