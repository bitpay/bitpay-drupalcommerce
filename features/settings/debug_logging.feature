Feature: Enable/Disable Debug Logging
  As a merchant of BitPay
  I want to set the debug log level
  So that I can get more information about errors


  Scenario: Debug logging is disabled
    Given I have debug logging set to disabled
    When I do something that might generate logs at several levels
    And I go look at the logs somehow
    Then I should see "error" level logs
    And I should see "warn" level logs
    But I should not see "info" level logs
    But I should not see "debug" level logs
    But I should not see "trace" level logs

  Scenario: Debug logging is enabled
    Given I have debug logging set to enabled
    When I do something that might generate logs at several levels
    And I go look at the logs somehow
    Then I should see "error" level logs
    And I should see "warn" level logs
    And I should see "info" level logs
    And I should see "debug" level logs
    And I should see "trace" level logs
