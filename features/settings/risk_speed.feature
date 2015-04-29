Feature: Risk/Speed Setting for Invoices
  As a merchant of BitPay
  I want to adjust the invoice risk/speed for my entire store
  So that I can consider invoice payments confirmed according to my risk preferences

  Background: Store is connected to BitPay
    Given store is connected to BitPay

  Scenario Outline: Risk/Speed
    Given I am on the plugin's general settings page
    When I set Risk/Speed to <level>
    And save settings
    And I create a new purchase
    And I choose BitPay as the payment option
    Then the BitPay invoice should have Risk/Speed set to <level>

  Examples:
    | level  |
    | low    |
    | medium |
    | high   |

