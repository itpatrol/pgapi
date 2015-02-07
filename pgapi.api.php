<?php
/**
 * @file
 * Provides hook documentation for PGAPI module.
 */

/**
 * Returns gateway info.
 *
 * @return array
 *   Keyed by module name.
 */
function hook_pgapi_gw_info() {
}

function hook_pgapi_callback($transaction) {
}

function hook_pgapi_format_price($type, $price, $symbol) {
}

function hook_pgapi_transaction($status, $transaction) {
}

function hook_pgapi_transaction_all($status, $transaction) {
}

/**
 * Execute pgapi_payment_page.
 *
 * Hook used for execute own code on payment page.
 *
 * @param object $transaction
 *   Payment object.
 */
function hook_pgapi_payment_page($transaction) {
  if (isset($_POST['status_failed'])) {
    $transaction->status = PG_FAILED;
    pgapi_transaction_save($transaction);
  }
}

/**
 * Executes pgapi hook.
 *
 * @param string $op
 *   The operation to be performed. Possible values:
 *   - "payment gateway info".
 *   - "display name".
 *   - "payment page".
 *   - "get form".
 *   - "process form".
 *   - "edit".
 *
 * @param number $a3
 *   Price.
 *
 * @param array $a4
 *   Form result.
 */
function hook_pgapi_gw($op, $a3 = NULL, $a4 = NULL) {
}

function hook_pgapi_transaction_status(&$status) {
}

function hook_pgapi_transaction_workflow(&$workflow) {
}
