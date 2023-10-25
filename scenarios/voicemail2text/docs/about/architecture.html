<!DOCTYPE html>

<html>

<head>

  <link rel="stylesheet" href="/docs/styles.css">

</head>


<body>

<h1>Architecture</h1>

<p>V2TIC is deployable in a container orchestration environment (for example, Kubernetes) to facilitate high availability, load-balancing, and autoscaling.  </p>

<h2>Overview of V2TIC request and response</h2>

<p>Like for Nuance V2T, V2TIC offers both HTTPS and SMTP protocols. Where V2T solutions connect to the on-premises Nuance Voice-to-Text Conversion Engine, V2TIC acts as an interface bewteen the voicemail server and Microsoft's Congitive Services Speech Recognition Service.</p>

<p>The diagram below shows the architecture and transcription process for V2TIC.</p>

<img src="/docs/images/V2TIC-diagram-full.png" alt="diagram of simplified V2TIC architecture">

<ol>
  <li>The profile, configuration, and depolyment information for your deployment are used to build the V2TIC instance. That information informs the V2TIC server, <tt>Common</tt>, and <tt>AcsClient</tt>.</li>
  <li>The V2TIC server for HTTPS or SMTP receives a request and audio for transcription. You can add certificate and key files for this connection. The server also processes the context (such as metadata) for the request.</li>
  <li>The scripts in the <tt>common</tt> folder creates the request using the context from the server and configuration from <tt>etc</tt>.</li>
  <li><tt>AcsClient</tt> prepares and sends the transcription request to your <a href="/docs/about/prerequisites.html">Azure AI endpoint</a>.</li>
  <li>Azure AI receives the request, completes the transcription, and returns a response.</li>
  <li><tt>AcsClient</tt> processes and converts the transcription response from Azure AI.</li>
  <li><tt>common</tt> takes the transcription response and context to build the final response.</li>
  <li><tt>common</tt> sends the response to the final destination. SMTP deployments can add certificate and key files to authenticate this connection. However, HTTPS deployments cannot, so make sure the end server does not block the port and IP address for HTTPS deployments.</li>
  <li>While running the <a href="/docs/build-deploy/build-summary.html">sample application</a>, the sample client applications send the request from within V2TIC.</li>
  <li>During testing, you can change this final connection to the <a href="/docs/test-validate/test-validate.html">V2TIC listener</a> to view the final response. You can't send to the listener and an outside server at the same time, so you must change the connection back after you finish testing.</li>
</ol>