<?php
/**
 * @file
 * Adds Bitcoin as a currency with currency conversion provided by BitPay.
 */

/**
 * Implements hook_commerce_currency_info().
 */
function commerce_bitpay_commerce_currency_info() {
  return array(
    'BTC' => array(
      'code' => 'BTC',
      'symbol' => 'B⃦',
      'name' => t('Bitcoin'),
      'code_placement' => 'after',
      'decimals' => 8,
      'minor_unit' => t('Satoshi'),
      'major_unit' => t('Bitcoin'),
      'conversion_callback' => 'commerce_bitpay_bitcoin_currency_convert',
    ),
  );
}

/**
 * Converts Bitcoin into a target currency using rates from BitPay.
 */
function commerce_bitpay_bitcoin_currency_convert($amount, $currency_code, $target_currency_code) {
  $currency = commerce_currency_load($currency_code);
  $target_currency = commerce_currency_load($target_currency_code);

  // First multiply the amount to accommodate differences in decimals between
  // the source and target currencies.
  $exponent = $target_currency['decimals'] - $currency['decimals'];
  $amount *= pow(10, $exponent);

  $response = drupal_http_request('https://bitpay.com/rates/' . $target_currency_code);
  if ($response->code == 200) {
    $bitpay_rate = json_decode($response->data, TRUE);
    return $amount * $bitpay_rate['data']['rate'];
  }

  return $amount;
};
