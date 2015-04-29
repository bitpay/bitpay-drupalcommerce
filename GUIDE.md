# Using the (name of plugin)

## Prerequisites
You must have a BitPay merchant account to use this plugin.  It's free to [sign-up for a BitPay merchant account](https://bitpay.com/start).

## Server Requirements

* [Drupal](https://www.drupal.org/requirements) >= 7
* [Drupal Commerce](https://drupalcommerce.org/download)
* [GMP](http://php.net/manual/en/book.gmp.php) or [BCMath](http://php.net/manual/en/book.bc.php) You may have to install GMP as most servers do not come with it, but generally BCMath is already included.
* [Mcrypt](http://us2.php.net/mcrypt)
* [OpenSSL](http://us2.php.net/openssl) Must be compiled with PHP
* [cURL](http://us2.php.net/openssl) Must be compiled with PHP
* PHP >= 5.4

## Installation

1. Go to Administration > Site Settings > Advanced Settings > Modules > Install new module
2. Choose the `commerce_bitpay.zip` that you have downloaded.
3. Click **Install**
4. Go to Administration > Site Settings > Advanced Settings > Modules

## Configuration
1. Check the checkbox next to Commerce BitPay
2. Click **Save Configuration**
3. Scroll back to Commerce Bitpay, and click the **Configure** link to go to the *BitPay Configuration Page*

### Connecting to BitPay
1. At the *BitPay Configuration Page*, click **Connect To BitPay Live**, or **Connect To BitPay Test**
2. Login at https://bitpay.com or https://test.bitpay.com if necessary
3. At BitPay's API Token Request page, click **Approve**

You should now be back at the *BitPay Configuration Page* on your Drupal Commerce site.
It should say `Connected to Livenet` or `Connected to Testnet`

## Usage
Once enabled, your customers will be given the option to pay with Bitcoin. Once
they checkout they are redirected to a full screen BitPay invoice to pay for
the order.

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
