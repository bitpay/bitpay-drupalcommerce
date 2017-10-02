# Using BitPay for Drupal Commerce

## Prerequisites
You must have a BitPay merchant account to use this plugin.  It's free to [sign-up for a BitPay merchant account](https://bitpay.com/start).

## Server Requirements

* [Drupal](https://www.drupal.org/requirements) >= 7
* [Drupal Commerce](https://drupalcommerce.org/download)
* [GMP](http://php.net/manual/en/book.gmp.php) or [BCMath](http://php.net/manual/en/book.bc.php) You may have to install GMP as most servers do not come with it, but generally BCMath is already included.
* [OpenSSL](http://us2.php.net/openssl) Must be compiled with PHP
* [cURL](http://us2.php.net/curl) Must be compiled with PHP and with SSL
* PHP >= 5.4

## Installation

1. Go to Administration > Site Settings > Advanced Settings > Modules > Install new module

   In **Install from a URL**, enter `https://github.com/bitpay/bitpay-drupalcommerce/releases`
   ![Install a new module](http://sambohler.github.io/drupalcommerce/install.png)
3. Click **Install**
4. Go to Administration > Site Settings > Advanced Settings > Modules
5. Type *bitpay* into the filter or search for the module in the **Commerce (contrib)** tab.
   ![Enable Commerce BitPay module](http://sambohler.github.io/drupalcommerce/enable.png)
6. Check **Enabled** and save then click **Save configuration**

## Configuration
1. Scroll back to Commerce Bitpay, and click the **Configure** link to go to the *BitPay Configuration Page*
   ![Configure Commerce BitPay](http://sambohler.github.io/drupalcommerce/configure.png)

### Connecting to BitPay
1. At the *BitPay Configuration Page*, click **Connect To BitPay Live**, or **Connect To BitPay Test**
   ![Connect to BitPay](http://sambohler.github.io/drupalcommerce/connect.png)
2. Login at https://bitpay.com or https://test.bitpay.com if necessary
3. At BitPay's API Token Request page, click **Approve**
   ![Approve API Token Request](http://sambohler.github.io/drupalcommerce/approve.png)

4. You should now be back at the *BitPay Configuration Page* on your Drupal Commerce site.
   It should say `Connected to Livenet` or `Connected to Testnet`
   ![Connected](http://sambohler.github.io/drupalcommerce/connected.png)

### Settings
1. Set your Risk/Speed or Advanced Settings and click **Save**

### Enable Payment Method
1. Go to Administration > Store Settings > Advanced store settings > Payment Methods
   ![Enable Payment Method](http://sambohler.github.io/drupalcommerce/enablepayment.png)
   Under **Disabled payment method rules** find **BitPay** and click **enable**
2. You are not ready to have customers pay with Bitcoin!


## Add BitCoin as a Currency
1. Go to Administration > Store Settings > Advanced store settings > Currency settings
   ![Currency settings](http://sambohler.github.io/drupalcommerce/currency.png)
   Expand **Enabled Currencies**, then scrolldown to *BTC - Bitcoin - B⃦* and check the box next to it
2. Click **Save configuration**
3. Once enabled, you will be able to set the price of products in BTC.  
   During checkout, price totals will be calculated via BitPay's BTC rate for your default store currency
   ![Pricing and currency conversion](http://sambohler.github.io/drupalcommerce/pricing.png)

## Usage
Once enabled, your customers will be given the option to pay with Bitcoin
![Checkout](http://sambohler.github.io/drupalcommerce/checkout.png)

Once they checkout they are redirected to a full screen BitPay invoice to pay for the order.
![BitPay Invoice](http://sambohler.github.io/drupalcommerce/invoice.png)

As a merchant, the orders in your Drupal Commerce store can be treated as any other
order. You may need to adjust the Invoice Settings depending on your order
fulfillment.

## How to Get Optimal Performance From the Plugin

It is highly recommended you install the GMP extension for PHP to acheive maximum performance when using this plugin.

### Compile PHP with GMP

[http://php.net/manual/en/gmp.installation.php](http://php.net/manual/en/gmp.installation.php)

### Enable Extension

If the extension has been included with your PHP install, you only need to uncomment the line in the PHP ini configuration file.

**On Windows:**

```ini
; From
;extension=php_gmp.dll
; To
extension=php_gmp.dll
```

**For Ubuntu:**

```bash
$ sudo apt-get update
$ sudo apt-get install php5-gmp
$ sudo php5enmod gmp

# Restart your server
```

**For Other Linux Systems:**

```ini
; From
;extension=gmp.so
; To
extension=gmp.so

# Restart your server
```

## Support

If you run into any issues after installing the plugin, you can search for help on [Commerce BitPay's lab on BitPay Labs](http://bitpaylabs.com/c/plugins/drupalcommerce) or send a Support Request to [BitPay Support](https://support.bitpay.com).
