Feature: Custom Order Statuses for BitPay Invoices
  As a merchant of BitPay
  I need to map bitpay invoice status to an order status
  So that I can find the status of orders in my system

  Background:
    Given I'm a merchant 
    And I have my risk/speed settings set to medium
    And I have matched bitpay invoice status paid to order status processing
    And I have matched bitpay invoice status confirmed to order status affirmed
    And I have matched bitpay invoice status complete to order status finished
    And I have a new order y with invoice x

  Scenario: A new invoice is paid 
    Given that invoice x is paid
    Then order y has status processing
    
  Scenario: A paid invoice is confirmed
    Given that invoice x is confirmed
    Then order y has status affirmed
    
  Scenario: A confirmed invoice is complete
    Given that invoice x is complete
    Then order y has status finished

