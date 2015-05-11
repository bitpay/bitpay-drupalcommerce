<?php

/**
 * @file
 * BitPay settings forms callbacks.
 */

/**
 * Redirect user to configuration page.
 */
function commerce_bitpay_configure() {

  // Check Payment UI is enabled.
  if (!module_exists('commerce_payment_ui')) {
    drupal_set_message(t('The Payment UI module is disabled - please enable it before configuring Commerce BitPay'), 'warning');
    drupal_goto('admin/modules');
  }

  // Load default payment rule configuration.
  $rule = rules_config_load('commerce_payment_commerce_bitpay');
  if (empty($rule)) {
    drupal_set_message(t('The default payment rule configuration cannot be found'), 'warning');
    drupal_goto('admin/commerce/config/payment-methods');
  }

  // Check first action for default payment rule configuration.
  $action = $rule->actions()->current();
  if (empty($action)) {
    drupal_set_message(t('No actions exist for the default payment rule configuration'), 'warning');
    drupal_goto(
    sprintf(
      'admin/commerce/config/payment-methods/manage/%d',
      $rule->id
    )
    );
  }

  // Redirect user to configuration page.
  drupal_goto(
    sprintf(
      'admin/commerce/config/payment-methods/manage/%s/edit/%d',
      $rule->name,
      $action->elementId()
    )
  );
}

/**
 * Generates keys and sends user to BitPay for pairing.
 */
function commerce_bitpay_connect($network) {

  // Check Payment UI is enabled.
  if (!module_exists('commerce_payment_ui')) {
    drupal_set_message(t('The Payment UI module is disabled - please enable it before configuring Commerce BitPay'), 'warning');
    drupal_goto('admin/modules');
  }

  // Load default payment rule configuration.
  $rule = rules_config_load('commerce_payment_commerce_bitpay');
  if (empty($rule)) {
    drupal_set_message(t('The default payment rule configuration cannot be found'), 'warning');
    drupal_goto('admin/commerce/config/payment-methods');
  }

  // Check first action for default payment rule configuration.
  $action = $rule->actions()->current();
  if (empty($action)) {
    drupal_set_message(t('No actions exist for the default payment rule configuration'), 'warning');
    drupal_goto(
      sprintf(
        'admin/commerce/config/payment-methods/manage/%d',
        $rule->id
      )
    );
  }
  $network_title = commerce_bitpay_set_network($network);
  commerce_bitpay_log('debug', 'Attempting to connect to ' . $network);
  $redirect = url('admin/settings/commerce-bitpay', array('absolute' => TRUE));
  try {
    $url = commerce_bitpay_get_pairing_url() . '&redirect=' . urlencode($redirect);
    drupal_goto($url);
  }
  catch (Exception $e) {
    commerce_bitpay_log('error', t('log_unable_to_connect'));
    drupal_goto('admin/settings/commerce-bitpay');
  }
}

/**
 * Erases keys and tokens used to connect with BitPay.
 */
function commerce_bitpay_disconnect() {
  commerce_bitpay_setting('connection', 'disconnected');
  commerce_bitpay_setting('network', NULL);
  commerce_bitpay_setting('token', NULL);
  commerce_bitpay_setting('pairing_code', NULL);
  commerce_bitpay_setting('pairing_expiration', NULL);
  drupal_goto('admin/settings/commerce-bitpay');
}
