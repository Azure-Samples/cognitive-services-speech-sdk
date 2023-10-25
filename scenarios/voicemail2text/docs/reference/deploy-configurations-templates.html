<!DOCTYPE html>

<html>

<head>

  <link rel="stylesheet" href="/docs/styles.css">

</head>


<body>

<h1>Configuration fields</h1>

<p>The V2TIC message request and conversion response formats are the same as for Nuance V2T. Configurations for these are stored in the <tt>configmap-file.yaml</tt> for individual deployments and globally in <tt>config.properties</tt>. <tt>configmap-file.yaml</tt> values override global settings.</p>

<p><b>Note</b>: You cannot make changes to an active V2TIC deployment. This is because building a deployment creates an instance of the configuration files and does not continuously update to new configurations. To make a change to a configuration, such as changing a port number, you must <a href="/docs/destroy-autoscale/destroy.html">destroy the active deployment</a> and <a href="/docs/build-deploy/build-summary.html">build a new image</a>.</p>

<h2>Global configuration config.properties</h2>

<ul>
    <li>[v2tic]: settings section for V2TIC environment such as logging, ports, etc.
    <ul>
        <li>logging_level=</li>
        <li>port=9443</li>
        <li>host=127.0.0.1</li>
        <li>cert_file=etc/certs/certificate.pem</li>
        <li>key_file=etc/certs/private_key.pem</li>
        <li>consume_request_timeout=5 seconds</li>
        <li>coroutine_execution_default_timeout=10 seconds</li>
    </ul>
    </li>
</ul>
    
<ul>
    <li>[transcoder]: Settings for transcoder in Linux.</li>
    <li>[language_configuration]: Supported languages.</li>
    <li>[ejector]: Time settings for delivery.</li>
    <li>[smtp]: Settings section for the SMTP response channel such host, port, and efault SMTP commands.</li>
    <li>[profiles]: Path and file name of the profile deployment.</li>
    <li>[acs_client]: Settings for Azure AI speech-to-text. Go to <a href="/docs/configure/configure-azure.html">Configure Azure</a> for additional configuration.
    <ul>
        <li>profanity_option= Specifies how to handle profanity in recognition results (masked, raw, removed.) Default is masked.</li>
        <li>word_level_confidence=False</li>
        <li>display_word_timings=False</li>
        <li>silence.use_default_values=False</li>
    </ul>
    </li>
    <li>The following silence properties are applied when silence.use_default_values=false
    <ul>
        <li>silence.segment_silence_timeout=1000 ms</li>
        <li>silence.initial_silence_timeout=5000 ms (The start of the audio stream contained only silence, and the service timed out while waiting for speech.)</li>
        <li>segmentation_forced_timeout=45000 ms (Azure AI recommended.)</li>
    </ul>
    </li>
</ul>

<h2>HTTPS configmap-file.yaml</h2>

    <ul>
        <li>apiVersion: v1</li>
        <li>kind: ConfigMap</li>
          <li>name: Name of the configmap. Must match the <tt>name</tt> under <tt>configMapRef</tt> in <tt>deployment.yaml</tt>.</li>
    </ul>

<ul>
    <li>v2tic_logging_level: Logging level</li>
    <li>v2tic_nodeport: Incoming port for receiving requests from voicemail server. Used for port forwarding as required. The range of valid ports is 30000-32767.</li>
    <li>v2tic_port: Outgoing port for sending final responses.</li>
    <li>v2tic_host: IP address of the V2TIC server.</li>
    <li>v2tic_cert_file: Location of the security certificate for incoming requests.</li>
    <li>v2tic_key_file: Location of the private key for incoming requests.</li>
    <li>v2tic_consume_request_timeout: Timeout for consuming incoming requests.</li>
    <li>v2tic_coroutine_execution_default_timeout: Default timeout.</li>
</ul>

<ul>
    <li>profiles_folder: Path to the profile folder for this deployment.</li>
    <li>profiles_profile: Location and class name of the profile.</li>
    
    <li>
    # only applicable in linux
    transcoder_terminate_after:
    </li>
    <li>
        # only applicable in linux .. 2 seconds after the initial 10
        transcoder_kill_after:
    </li>
    
    metadata_default_language: Default language for transcription.
    <li>metadata_default_lid_languages: If language ID is enabled in acs settings, the list of applicable languages.</li>
    <li>metadata_default_thresholds: The minimum confidence level for transcription success and the maximum length of allowed transcription.</li>
    <li>metadata_default_lid_thresholds: In the case of multiple languages, the minimum confidence level for transcription success and the maximum length of allowed transcription.</li>
    <li>metadata_log_transcriptions_enabled: Whether to log transcriptions.</li>
    <li>metadata_truncate_lenghty_transcriptions_enabled: Whether to cut off a transcription once it's reached the maximum length.</li>
    <li>metadata_max_transcription_length: Maximum character length of transcription.</li>
</ul>

<ul>
    <li>ejector_max_attempts: Maximum allowed ejector attempts.</li>
    <li>ejector_wait_exponential_multiplier: "2"</li>
    <li>ejector_wait_exponential_min: 0.5 seconds</li>
    <li>ejector_wait_exponential_max: 2 seconds</li>
    <li># ejector_retry_on_exception example for http exceptions:OSError,http.client.HTTPException</li>
    <li># ejector_retry_on_exception example for smtp exceptions:ValueError,aiosmtplib.SMTPException</li>
    <li>ejector_retry_on_exception: Whether to retry ejector when an exception occurs.</li>
</ul>

<p>For Azure AI (<tt>acs_client</tt>) fields, go to <a href="/docs/configure/configure-azure.html">Azure AI configurations.</a></p>

<h2>SMTP configmap-file.yaml</h2>

    <ul>
        <li>apiVersion: v1</li>
        <li>kind: ConfigMap</li>
          <li>name: Name of the configmap. Must match the <tt>name</tt> under <tt>configMapRef</tt> in <tt>deployment.yaml</tt>.</li>
    </ul>

<ul>
    <li>v2tic_logging_level: Logging level</li>
    <li>v2tic_nodeport: Incoming port for receiving requests from voicemail server. Used for port forwarding as required. The range of valid ports is 30000-32767.</li>
    <li>v2tic_port: Outgoing port for sending final responses.</li>
    <li>v2tic_host: IP address of the V2TIC server.</li>
    <li>v2tic_cert_file: Location of the security certificate for incoming requests.</li>
    <li>v2tic_key_file: Location of the private key for incoming requests.</li>
    <li>v2tic_consume_request_timeout: Timeout for consuming incoming requests.</li>
    <li>v2tic_coroutine_execution_default_timeout: Default timeout.</li>
</ul>

<ul>
<li>smtp_client_cert_file: Location of the security certificate for outgoing responses.</li>
<li>smtp_client_key_file: Location of the key for outgoing responses.</li>
<li>smtp_response_port: Port for sending final responses.</li>
<li>smtp_response_host: IP address for sending the final response.</li>
<li>smtp_default_response_from: Default value for "from" of the final response (such as an email sender).</li>
<li>smtp_default_response_subject: Subject for the final response.</li>
</ul>

<ul>
    <li>profiles_folder: Path to the profile folder for this deployment.</li>
    <li>profiles_profile: Location and class name of the profile.</li>
    
    <li>
    # only applicable in linux
    transcoder_terminate_after:
    </li>
    <li>
        # only applicable in linux .. 2 seconds after the initial 10
        transcoder_kill_after:
    </li>
    
    metadata_default_language: Default language for transcription.
    <li>metadata_default_lid_languages: If language ID is enabled in acs settings, the list of applicable languages.</li>
    <li>metadata_default_thresholds: The minimum confidence level for transcription success and the maximum length of allowed transcription.</li>
    <li>metadata_default_lid_thresholds: In the case of multiple languages, the minimum confidence level for transcription success and the maximum length of allowed transcription.</li>
    <li>metadata_log_transcriptions_enabled: Whether to log transcriptions.</li>
    <li>metadata_truncate_lenghty_transcriptions_enabled: Whether to cut off a transcription once it's reached the maximum length.</li>
    <li>metadata_max_transcription_length: Maximum character length of transcription.</li>
</ul>

<ul>
    <li>ejector_max_attempts: Maximum allowed ejector attempts.</li>
    <li>ejector_wait_exponential_multiplier: "2"</li>
    <li>ejector_wait_exponential_min: 0.5 seconds</li>
    <li>ejector_wait_exponential_max: 2 seconds</li>
    <li># ejector_retry_on_exception example for http exceptions:OSError,http.client.HTTPException</li>
    <li># ejector_retry_on_exception example for smtp exceptions:ValueError,aiosmtplib.SMTPException</li>
    <li>ejector_retry_on_exception: Whether to retry ejector when an exception occurs.</li>
</ul>

<p>For Azure AI (<tt>acs_client</tt>) fields, go to <a href="/docs/configure/configure-azure.html">Azure AI configurations.</a></p>

<p><a href="/docs/index.html">Return to table of contents</a></p>
</body></html>