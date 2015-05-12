<?php
/**
 * @file
 * Handle IPNs from BitPay.
 */

/**
 * Updates the status for a transaction from an IPN.
 */
function commerce_bitpay_ipn_handler($order, $transaction) {

  $post = file_get_contents('php://input');

  // No data.
  if (empty($post)) {
    watchdog('commerce_bitpay', 'IPN handler called with no data', array(), WATCHDOG_WARNING);
    return;
  }

  // Invalid JSON.
  $json = @json_decode($post, TRUE);
  if (empty($json)) {
    commerce_bitpay_log('warn', 'IPN handler called with invalid JSON data');
    return;
  }
  if (!array_key_exists('id', $json)) {
    commerce_bitpay_log('warn', 'IPN handler called with data missing `id` field');
    return;
  }
  if (!array_key_exists('url', $json)) {
    commerce_bitpay_log('warn', 'IPN handler called with data missing `url` field');
    return;
  }

  // Try to set the network based on the url first since the merchant may have
  // switched networks while test invoices are still being confirmed.
  $network = NULL;
  if (FALSE !== strpos($json['url'], 'https://test.bitpay.com')) {
    $network = COMMERCE_BITPAY_MODE_TEST;
  }
  else {
    $network = COMMERCE_BITPAY_MODE_LIVE;
  }
  $invoice = commerce_bitpay_get_invoice($json['id'], $network);
  $transaction = commerce_payment_transaction_load_multiple(array(), array('order_id' => $invoice->getOrderId()), TRUE);

  // Update the order and transaction.
  switch ($invoice->getStatus()) {
    case 'paid':
      commerce_checkout_complete($order);
      $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
      $transaction->message = t('Invoice paid, but awaiting confirmation.');
      break;

    case 'confirmed':
      commerce_checkout_complete($order);
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->message = t('Invoice payment confirmed.');
      break;

    case 'complete':
      commerce_checkout_complete($order);
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->message = t('Invoice payment complete.');
      break;

    default:
      return;
  }

  // Add debugging details.
  $transaction->remote_status = $invoice->getStatus();
  $transaction->payload = var_export($invoice, TRUE);

  // Save the transaction.
  commerce_payment_transaction_save($transaction);
}
