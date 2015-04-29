<?php

/**
 * @file
 * Handle IPNs from BitPay.
 */

/**
 * Prepares a BitPay invoice from a Drupal Commerce Order
 */
function commerce_bitpay_prepare_invoice($order) {
  // Get order to wrapper.
  $wrapper = entity_metadata_wrapper('commerce_order', $order);

  // Bitpay Invoice
  $invoice = new \Bitpay\Invoice();

  // Order ID
  commerce_bitpay_log('info', sprintf('Preparing Invoice for Order %s', (string)$order->order_id));
  $invoice->setOrderId((string)$order->order_id);

  // Currency
  $order_total = $wrapper->commerce_order_total->value();
  if (empty($order_total['currency_code'])) {
    $this->log('error', 'Cannot prepare invoice without `currency_code`');
    throw Exception('Cannot prepare invoice without `currency_code`');
  }
  $invoice->setCurrency(new \Bitpay\Currency($order_total['currency_code']));

  // Price
  $amount = $wrapper->commerce_order_total->amount->value();
  $price = commerce_currency_amount_to_decimal($amount, $order_total['currency_code']);
  $invoice->setPrice((float)$price);

  // Physical Item
  $shipping_info = $wrapper->commerce_customer_shipping->value();
  $shipping_services = commerce_shipping_services();
  if (!empty($shipping_info->commerce_customer_address)) {
    foreach ($order_total['data']['components'] as $component) {
      foreach ($shipping_services as $service) {
        if (isset($service['price_component']) && $service['price_component'] == $component['name']) {
          $invoice->getItem()->setPhysical(true);
          break 2;
        }
      }
    }
  }

  // Buyer Information
  $buyer = new \Bitpay\Buyer();
  $buyer_info = $wrapper->commerce_customer_billing->value();
  if (!empty($buyer_info->commerce_customer_address)) {
    list (, $buyer_address) = each($buyer_info->commerce_customer_address);
    if (!empty($buyer_address)) {
      $buyer_name     = explode(' ', $buyer_address[0]['name_line'], 2);
      $buyer_first    = empty($buyer_name[0]) ? '' : substr($buyer_name[0], 0, 100);
      $buyer_last     = empty($buyer_name[1]) ? '' : substr($buyer_name[1], 0, 100);
      $buyer_address1 = empty($buyer_address[0]['thoroughfare']) ? '' : substr($buyer_address[0]['thoroughfare'], 0, 100);
      $buyer_address2 = empty($buyer_address[0]['premise']) ? '' : substr($buyer_address[0]['premise'], 0, 100);
      $buyer_city     = empty($buyer_address[0]['locality']) ? '' : substr($buyer_address[0]['locality'], 0, 100);
      $buyer_state    = empty($buyer_address[0]['administrative_area']) ? '' : substr($buyer_address[0]['administrative_area'], 0, 100);
      $buyer_zip      = empty($buyer_address[0]['postal_code']) ? '' : substr($buyer_address[0]['postal_code'], 0, 100);
      $buyer_country  = empty($buyer_address[0]['country']) ? '' : substr($buyer_address[0]['country'], 0, 100);
      $buyer->setFirstName($buyer_first)
        ->setLastName($buyer_last)
        ->setAddress(array($buyer_address1, $buyer_address2))
        ->setCity($buyer_city)
        ->setState($buyer_state)
        ->setZip($buyer_zip)
        ->setCountry($buyer_country);
    }
  }
  $buyer_email = empty($order->mail) ? '' : substr($order->mail, 0, 100);
  $buyer->setEmail($buyer_email);
  $invoice->setBuyer($buyer);

  // Full Notifications has to be true for IPNs to come for each state update.
  $invoice->setFullNotifications(true);

  // Return URL
  global $base_url;
  $return_url = commerce_bitpay_setting('return_url');
  if (empty($return_url)) {
    $return_url = $base_url . '/checkout/%commerce_order/complete';
  }
  $return_url = str_replace('%commerce_order', $order->order_id, $return_url);
  $invoice->setRedirectUrl($return_url);

  // Notification URL
  $notify_url = commerce_bitpay_setting('notify_url');
  if (empty($notify_url)) {
    $notify_url = $base_url . '/bitpay/ipn';
  }
  $invoice->setNotificationUrl($notify_url);

  // Risk/Speed
  $invoice->setTransactionSpeed(commerce_bitpay_setting('risk_speed'));

  return $invoice;
}

/**
 * Redirect payment.
 */
function commerce_bitpay_redirect_form($form, &$form_state, $order, $payment_method) {
  $transaction = commerce_payment_transaction_new('bitpay', $order->order_id);
  try {
    $invoice = commerce_bitpay_prepare_invoice($order);
		$invoice = commerce_bitpay_create_invoice($invoice);

    // Create a transaction
    $transaction = commerce_payment_transaction_new('commerce_bitpay', $order->order_id);
    $transaction->instance_id   = $payment_method['instance_id'];
    $transaction->currency_code = $invoice->getCurrency()->getCode();
    $transaction->amount        = $invoice->getPrice();
    $transaction->remote_id     = $invoice->getId();
    $transaction->remote_status = $invoice->getStatus();
    $transaction->message       = $invoice->getUrl();
    commerce_payment_transaction_save($transaction);

	} catch (Exception $e) {
    drupal_set_message(t('Sorry, but there was a problem communicating with BitPay for Bitcoin checkout.', array(), 'error'));
		return;
	}
  drupal_goto($invoice->getUrl());
}
