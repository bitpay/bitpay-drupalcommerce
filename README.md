# BitPay for Drupal Commerce (Commerce BitPay)

[![Latest Version](https://img.shields.io/github/release/bitpay/bitpay-drupalcommerce.svg?style=flat-square)](https://github.com/bitpay/bitpay-drupalcommerce/releases)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square)](LICENSE.md)
[![Build Status](https://travis-ci.org/bitpay/bitpay-drupalcommerce.svg?branch=master)](https://travis-ci.org/bitpay/bitpay-drupalcommerce)
[![Join the chat at https://gitter.im/bitpay/plugin](http://sambohler.github.io/gitter.svg)](https://gitter.im/bitpay/bitpay-drupalcommerce?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## Plugin Installation, Configuration, and Usage
Follow the instructions found in the [Commerce BitPay Guide](GUIDE.md)

## Development Setup

``` bash
# Clone the repo
$ git clone https://github.com/bitpay/bitpay-drupalcommerce.git
$ cd ./bitpay-drupalcommerce

# Install dependencies via Composer
$ composer install
```

## Development Workflow

``` bash
# Watch for source code changes and copy them to ./www
# You could symlink the commerce_bitpay module in your local web server
# to ./www for development purposes
$ ./bin/robo watch
```

## Testing

``` bash
$ ./bin/robo test
```

## Build

``` bash
$ ./bin/robo build

# Outputs:
# ./build/commerce-bitpay/ - the distribution files
# ./build/commerce-bitpay.zip - the distribution archive
```

## Contributing

Please see [CONTRIBUTING](CONTRIBUTING.md) for details.

## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.
