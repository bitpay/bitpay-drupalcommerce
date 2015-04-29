<?php
/**
 * @file
 * BitPay library calls and helpers.
 */

if (!class_exists('Bitpay/Autoloader')) {
  // Load up the BitPay library.
  $autoloader = __DIR__ . '/../libraries/Bitpay/Autoloader.php';
  if (TRUE === file_exists($autoloader) && TRUE === is_readable($autoloader)) {
    require_once $autoloader;
    \Bitpay\Autoloader::register();
  }
  else {
    throw new Exception('BitPay Library could not be loaded');
  }
}

/**
 * Encrypt data using fingerprint key.
 */
function commerce_bitpay_encrypt($data) {

  if (FALSE === isset($data) || TRUE === empty($data)) {
    throw new \Exception('The Bitpay payment plugin was called to encrypt data but no data was passed!');
  }
  $mcrypt_ext  = new \Bitpay\Crypto\McryptExtension();
  $fingerprint = substr(sha1(sha1(__DIR__)), 0, 24);
  if (FALSE === isset($fingerprint) || TRUE === empty($fingerprint)) {
    throw new \Exception('The Bitpay payment plugin was called to encrypt data but could not generate a fingerprint parameter!');
  }
  $encrypted = $mcrypt_ext->encrypt(base64_encode($data), $fingerprint, '00000000');
  if (TRUE === empty($encrypted)) {
    throw new \Exception('The Bitpay payment plugin was called to encrypt data but failed');
  }

  return $encrypted;
}

/**
 * Decrypt data using fingerprint key.
 */
function commerce_bitpay_decrypt($encrypted) {

  if (FALSE === isset($encrypted) || TRUE === empty($encrypted)) {
    throw new \Exception('The Bitpay payment plugin was called to decrypt data but no data was passed!');
  }
  $mcrypt_ext  = new \Bitpay\Crypto\McryptExtension();
  $fingerprint = substr(sha1(sha1(__DIR__)), 0, 24);
  if (FALSE === isset($fingerprint) || TRUE === empty($fingerprint)) {
    throw new \Exception('The Bitpay payment plugin was called to decrypt data but could not generate a fingerprint parameter!');
  }
  $decrypted = base64_decode($mcrypt_ext->decrypt($encrypted, $fingerprint, '00000000'));
  // Strict base64 char check.
  if (FALSE === base64_decode($decrypted, TRUE)) {
    $error_string .= 'Data appears to have already been decrypted. Strict base64 check failed.';
  }
  else {
    $decrypted = base64_decode($decrypted);
  }
  if (TRUE === empty($decrypted)) {
    throw new \Exception('The Bitpay payment plugin was called to unserialize a decrypted object and failed! The decrypt function was called with "' . $encrypted . '"');
  }

  return $decrypted;
}

/**
 * Generates a token and returns a link to pair with it on BitPay.
 *
 * @return string
 *   BitPay pairing url for connection token
 */
function commerce_bitpay_get_pairing_url() {

  // Sanitize label.
  $label = preg_replace('/[^a-zA-Z0-9 \-\_\.]/', '', variable_get('site_name'));
  $label = substr('DrupalCommerce - ' . $label, 0, 59);
  // Generate a new Client ID each time.
  // Prevents multiple pair, and revoked token before return issues.
  commerce_bitpay_generate_id();
  $client = commerce_bitpay_get_client();
  $token  = $client->createToken(array(
        'facade' => 'merchant',
        'label' => $label,
        'id' => (string) commerce_bitpay_get_public_key()->getSin(),
    ));
  commerce_bitpay_setting('connection', 'connecting');
  commerce_bitpay_setting('token', $token->getToken());
  $network = commerce_bitpay_get_network();
  if (443 === $network->getApiPort()) {
    return 'https://' . $network->getApiHost() . '/api-access-request?pairingCode=' . $token->getPairingCode();
  }
  else {
    return 'http://' . $network->getApiHost() . ':' . $network->getApiPort() . '/api-access-request?pairingCode=' . $token->getPairingCode();
  }
}

/**
 * Checks to see if we've successfully connected to BitPay's API.
 */
function commerce_bitpay_check_connection() {

  $connection_token = commerce_bitpay_setting('token');

  try {
    $client = commerce_bitpay_get_client();
    $tokens = $client->getTokens();
    foreach ($tokens as $token) {
      if ($token->getToken() === $connection_token) {
        commerce_bitpay_setting('connection', 'connected');
        commerce_bitpay_setting('pairing_code', NULL);
        commerce_bitpay_setting('pairing_expiration', NULL);

        return;
      }
    }
  }
  catch (\UnexpectedValueException $e) {
    commerce_bitpay_log('error', 'log_connection_key');
  }
  catch (\Exception $e) {
    // An exception is raised when no tokens can be found
    // This is fine and expected.
  }

  // Disconnect if token doesn't exist anymore.
  if (commerce_bitpay_setting('connection') === 'connected') {
    commerce_bitpay_setting('connection', 'disconnected');
    commerce_bitpay_setting('token', NULL);
    commerce_bitpay_setting('network', NULL);
    commerce_bitpay_setting('status', '0');
  }
}

/**
 * Generates a new set of keys to interact with BitPay's API.
 *
 * @return \Bitpay\PrivateKey
 *   Private key used to interact with BitPay's API.
 */
function commerce_bitpay_generate_id() {

  // Generate new keys.
  $private_key = new Bitpay\PrivateKey();
  $private_key->generate();
  $public_key = $private_key->getPublicKey();

  // Persist the keys to the database.
  commerce_bitpay_setting('private_key', commerce_bitpay_encrypt(serialize($private_key)));
  commerce_bitpay_setting('public_key', commerce_bitpay_encrypt(serialize($public_key)));
  commerce_bitpay_setting('connection', 'disconnected');
  commerce_bitpay_setting('token', NULL);

  return $private_key;
}

/**
 * Retrieves a client to interact with BitPay's API.
 *
 * @param string $network
 *   Optional network identifier.
 *
 * @return \Bitpay\Client
 *   BitPay API Client
 */
function commerce_bitpay_get_client($network = NULL) {

  $network = commerce_bitpay_get_network($network);

  $curl_options = array();
  if ($network instanceof Bitpay\Network\Customnet) {

    // Customize the curl options.
    $curl_options = array(
      CURLOPT_SSL_VERIFYPEER => FALSE,
      CURLOPT_SSL_VERIFYHOST => FALSE,
    );
  }
  $adapter = new Bitpay\Client\Adapter\CurlAdapter($curl_options);

  $private_key = commerce_bitpay_get_private_key();
  $public_key  = commerce_bitpay_get_public_key();

  $client = new Bitpay\Client\Client();
  $client->setPrivateKey($private_key);
  $client->setPublicKey($public_key);
  $client->setNetwork($network);
  $client->setAdapter($adapter);

  return $client;
}

/**
 * Validates and sets the network setting.
 *
 * @param string|array $network
 *   Network to connect to or custom Network parameter array.
 *
 * @return \Bitpay\Network\NetworkInterface
 *   BitPay API Network
 */
function commerce_bitpay_set_network($network) {

  if ($network === 'livenet') {
    commerce_bitpay_setting('network', 'livenet');

    return 'Livenet';
  }
  elseif ($network === 'testnet') {
    commerce_bitpay_setting('network', 'testnet');

    return 'Testnet';
  }
  elseif (is_array($network)) {
    if (isset($network['url']) && isset($network['port'])) {
      $port_required  = (isset($network['port_required'])) ? TRUE : FALSE;
      $network_params = array(
        'url' => $network['url'],
        'port' => $network['port'],
        'port_required' => $port_required,
      );
      commerce_bitpay_setting('network', $network_params);

      return 'Customnet';
    }
  }
  commerce_bitpay_setting('network', 'livenet');

  return 'Livenet';
}

/**
 * Retrieves a BitPay's API Network object.
 *
 * @param string $network
 *   Optional network identifier.
 *
 * @return \Bitpay\Network\NetworkInterface
 *   BitPay API Network
 */
function commerce_bitpay_get_network($network = NULL) {

  $network = (empty($network)) ? commerce_bitpay_setting('network') : $network;
  commerce_bitpay_log('trace', 'Getting Network: ' . var_export($network, TRUE));
  if ($network === 'livenet') {
    return new Bitpay\Network\Livenet();
  }
  elseif ($network === 'testnet') {
    return new Bitpay\Network\Testnet();
  }
  elseif (is_array($network)) {
    if (isset($network['url']) && isset($network['port']) && isset($network['port_required'])) {
      return new Bitpay\Network\Customnet($network['url'], (int) $network['port'], (boolean) $network['port_required']);
    }
  }
  commerce_bitpay_setting('network', 'livenet');

  return new Bitpay\Network\Livenet();
}

/**
 * Retrieves the private key used to interact with BitPay's API.
 *
 * @throws UnexpectedValueException
 *   If the key cannot be decrypted and unserialized to a PrivateKey object.
 *
 * @return \Bitpay\PrivateKey
 *   Private key used to interact with BitPay's API.
 */
function commerce_bitpay_get_private_key() {

  $private_key = commerce_bitpay_setting('private_key');

  // Null check the private key, and generate it should it not exist.
  if (is_null($private_key)) {
    commerce_bitpay_log('info', 'log_no_private_key');

    return commerce_bitpay_generate_id();
  }

  commerce_bitpay_log('debug', 'log_private_key_found');
  $private_key = @unserialize(commerce_bitpay_decrypt($private_key));

  // Check for key integrity.
  if (!($private_key instanceof Bitpay\PrivateKey)) {
    commerce_bitpay_log('error', sprintf('log_private_key_wrong_type', gettype($private_key)));
    commerce_bitpay_log('trace', sprintf('log_encrypted_key', $private_key));
    commerce_bitpay_log('trace', sprintf('log_decrypted_key', commerce_bitpay_decrypt($private_key)));
    throw new UnexpectedValueException();
  }

  return $private_key;
}

/**
 * Retrieves the public key used to interact with BitPay's API.
 *
 * @return \Bitpay\PublicKey
 *   Public key used to interact with BitPay's API.
 */
function commerce_bitpay_get_public_key() {

  $public_key = commerce_bitpay_setting('public_key');

  // Null check the private key, and generate it should it not exist.
  if (is_null($public_key)) {
    commerce_bitpay_log('info', 'log_no_public_key');
    $private_key = commerce_bitpay_get_private_key();

    return $private_key->getPublicKey();
  }

  commerce_bitpay_log('debug', 'log_public_key_found');
  $public_key = @unserialize(commerce_bitpay_decrypt($public_key));

  // Check for key integrity.
  if (!($public_key instanceof Bitpay\PublicKey)) {
    commerce_bitpay_log('error', sprintf('log_public_key_wrong_type'), gettype($public_key));
    commerce_bitpay_log('trace', sprintf('log_encrypted_key'), $public_key);
    commerce_bitpay_log('trace', sprintf('log_decrypted_key'), commerce_bitpay_decrypt($public_key));

    // Try to fix the problem by regenerating it.
    commerce_bitpay_log('debug', 'log_public_key_regenerate');
    $private_key = commerce_bitpay_get_private_key();
    $public_key  = $private_key->getPublicKey();
    commerce_bitpay_log('warn', 'log_regenerate_success');
    commerce_bitpay_setting('public_key', commerce_bitpay_encrypt(serialize($public_key)));
  }

  return $public_key;
}

/**
 * Constructs some helpful diagnostic info.
 *
 * @return string
 *   Diagnostic information.
 */
function commerce_bitpay_get_server_info() {

  $gmp    = extension_loaded('gmp') ? 'enabled' : 'missing';
  $bcmath = extension_loaded('bcmath') ? 'enabled' : 'missing';
  $info   = "<pre><strong>Server Information:</strong>\n" . 'PHP: ' . phpversion() . "\n" . 'PHP-GMP: ' . $gmp . "\n" . 'PHP-BCMATH: ' . $bcmath . "\n" . 'Drupal: ' . VERSION . "\n" . 'BitPay Plugin: ' . COMMERCE_BITPAY_VERSION . "\n" . "BitPay Lib: {{bitpay_lib_version}}\n";

  return $info;
}

/**
 * Sends a support request to BitPay.
 */
function commerce_bitpay_send_support_request() {

  // $this->load->library('mail');
  //
  // $mail = new Mail(array(
  // 'protocol' => $this->config->get('config_mail')['protocol'],
  // 'parameter' => $this->config->get('config_mail')['parameter'],
  // 'hostname' => $this->config->get('config_mail')['smtp_hostname'],
  // 'username' => $this->config->get('config_mail')['smtp_username'],
  // 'password' => $this->config->get('config_mail')['smtp_password'],
  // 'port' => $this->config->get('config_mail')['smtp_port'],
  // 'timeout' => $this->config->get('config_mail')['smtp_timeout']
  // ));
  //
  // $mail->setTo('support@bitpay.com');
  // $mail->setFrom($this->request->post['request_email_address']);
  // $mail->setSender($this->request->post['request_name']);
  // $mail->setSubject($this->request->post['request_subject']);
  //
  // $description = $this->request->post['request_description'];
  //
  // Include server info?
  // if ($this->request->post['request_send_server_info'] === "1") {
  // $description .= "\n\n" . $this->getServerInfo();
  // }
  // $mail->setHtml($description);
  //
  // Include BitPay logs?
  // if ($this->request->post['request_send_logs'] === "1") {
  // $mail->addAttachment(DIR_LOGS . 'bitpay.log');
  // }
  //
  // $mail->send();
}

/**
 * Creates the BitPay Invoice from a prepared Invoice.
 *
 * @param \Bitpay\Invoice $invoice
 *   Prepared invoice to send to BitPay.
 *
 * @return \Bitpay\Invoice
 *   Complete invoice returned by BitPay.
 */
function commerce_bitpay_create_invoice(\Bitpay\Invoice $invoice) {

  commerce_bitpay_log('info', 'Attempting to generate invoice for ' . $invoice->getOrderId() . '...');

  $token = new \Bitpay\Token();
  $token->setToken(commerce_bitpay_setting('token'));

  $client = commerce_bitpay_get_client();
  $client->setToken($token);

  return $client->createInvoice($invoice);
}

/**
 * Retrieves a BitPay Invoice by ID.
 *
 * @param string $invoice_id
 *   BitPay Invoice ID.
 * @param string $network
 *   Optional network identifier.
 *
 * @return \Bitpay\Invoice
 *   BitPay Invoice.
 */
function commerce_bitpay_get_invoice($invoice_id, $network = NULL) {

  commerce_bitpay_log('info', 'Attempting to retrieve invoice for ' . $invoice_id . '...');

  $token = new \Bitpay\Token();
  $token->setToken(commerce_bitpay_setting('token'));

  $client = commerce_bitpay_get_client($network);
  $client->setToken($token);

  return $client->getInvoice($invoice_id);
}

/**
 * Logs with an arbitrary level.
 *
 * @param string $level
 *   The type of log.
 *   Should be 'error', 'warn', 'info', 'debug', 'trace'.
 *   In normal mode, 'error' and 'warn' are logged.
 *   In debug mode, all are logged.
 * @param string $message
 *   The message of the log.
 * @param int $depth
 *   How deep to go to find the calling function.
 */
function commerce_bitpay_log($level, $message, $depth = 0) {

  $level  = strtoupper($level);
  $prefix = '[' . $level . ']';

  // Debug formatting.
  if (commerce_bitpay_setting('debug') === '1') {
    $depth += 1;
    $prefix .= '{';
    $backtrace = debug_backtrace();
    if (isset($backtrace[$depth]['class'])) {
      $class = preg_replace('/[a-z]/', '', $backtrace[$depth]['class']);
      $prefix .= $class . $backtrace[$depth]['type'];
    }
    if (isset($backtrace[$depth]['function'])) {
      $prefix .= $backtrace[$depth]['function'];
    }
    $prefix .= '}';
  }

  switch ($level) {
    case 'ERROR':
            $drupal_level = WATCHDOG_ERROR;
      break;

    case 'WARN':
            $drupal_level = WATCHDOG_WARNING;
      break;

    case 'INFO':
            $drupal_level = WATCHDOG_INFO;
      break;

    case 'DEBUG':
        case 'TRACE':
            $drupal_level = WATCHDOG_DEBUG;
      break;

    default:
            $drupal_level = WATCHDOG_NOTICE;
  }

  if ('ERROR' === $level || 'WARN' === $level || commerce_bitpay_setting('debug') === '1') {
    watchdog('commerce_bitpay', $prefix . ' ' . $message, array(), $drupal_level);
  }
}

/**
 * Better setting method for bitpay settings.
 *
 * @param string $key
 *   Setting key.
 * @param string $value
 *   Setting value if setting the value.
 *
 * @return mixed
 *   Setting value, or void if setting the value.
 */
function commerce_bitpay_setting($key, $value = NULL) {

  // Normalize key.
  $key = 'bitpay_' . $key;

  // Set the setting.
  if (func_num_args() === 2) {
    variable_set($key, $value);
    return;
  }

  // Get the setting.
  if (in_array($key, ['risk_speed', 'debug', 'return_url', 'notify_url'])) {

    $settings = commerce_payment_method_instance_load('commerce_bitpay|commerce_payment_commerce_bitpay')['settings'];
    $settings['debug'] = $settings['advanced']['debug_logging'];
    $settings['return_url'] = $settings['advanced']['url_return'];
    $settings['notify_url'] = $settings['advanced']['url_notification'];

    return $settings[$key];

  }
  return variable_get($key);
}

/**
 * Updates BitPay connection status.
 */
function commerce_bitpay_connected() {

  if (commerce_bitpay_setting('connection') !== 'disconnected' && commerce_bitpay_setting('connection') !== NULL) {
    $was_connected = (commerce_bitpay_setting('connection') === 'connected');
    $was_network   = commerce_bitpay_setting('network');
    $was_network   = (is_array($was_network)) ? 'testnet' : $was_network;
    commerce_bitpay_check_connection();
    // Connection attempt successful!
    if (!$was_connected && commerce_bitpay_setting('connection') === 'connected') {
      $network_title = ucwords(commerce_bitpay_setting('network'));
    }

    return;
  }
}
