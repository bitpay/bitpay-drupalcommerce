Feature: Send a Support Request
  As a customer of bitpay
  I want to easily create support requests for bitpay plugins
  So that I can get help with my issue

  Scenario: I have chosen to send logs with my support request
    Given I am on the admin page for bitpay
    When I make a support request and include logs
    Then it will have logs

  Scenario: I have chosen not to send logs
    Given I am on the admin page
    When I make a support request and do not include logs
    Then it will not have logs

  Scenario: I have chosen to send server information with my support request
    Given I am on the admin page for bitpay
    When I make a support request and include server information
    Then it will have server information

  Scenario: I have chosen not to send server information
    Given I am on the admin page
    When I make a support request and do not include server information
    Then it will not have server information
