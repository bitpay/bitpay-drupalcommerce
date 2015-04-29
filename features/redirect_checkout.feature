Feature: Redirect Checkout Flow
  As a customer
  I want to be sent to BitPay during checkout
  So that I can pay an invoice I know to be from BitPay

  Scenario: Customer is buying something
    Given I'm a customer
    And I have completed the order process up to the checkout page
    When I choose the BitPay payment option
    Then I should be sent to a BitPay invoice for my order