Feature: Direct Checkout Flow
  As a customer
  I want the BitPay invoice to be on the checkout page
  So that I can pay for my order without leaving the merchant site

  Scenario: Customer is buying something
    Given I'm a customer
    And I have completed the order process up to the checkout page
    When I choose the BitPay payment option
    Then I should see an invoice on the checkout page