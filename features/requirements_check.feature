Feature: Requirements Check
  As a merchant
  I want the plugin to check my system for its requirements
  So that it only installs if I can actually use it

  Scenario: System meets requirements
    Given I'm a merchant
    And my system meets the plugin's requirements
    When I try to install the plugin
    Then it should install without errors

  Scenario: System doesn't meet requirements
    Given I'm a merchant
    And my system doesn't meet the plugin's requirements
    When I try to install the plugin
    Then it should halt the installation process
    And give me an error message about not meeting requirements