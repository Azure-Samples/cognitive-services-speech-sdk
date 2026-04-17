<?php
header('Access-Control-Allow-Origin: ' . $_SERVER['SERVER_NAME']);

// Replace with your own subscription key and service endpoint.
$subscriptionKey = 'YourSubscriptionKey';
$endpoint = 'YourServiceEndpoint'; // e.g., "https://my-speech-resource.cognitiveservices.azure.com"

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $endpoint . '/sts/v1.0/issueToken');
curl_setopt($ch, CURLOPT_POST, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, '{}');
curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json', 'Ocp-Apim-Subscription-Key: ' . $subscriptionKey)); 
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
echo curl_exec($ch);
?>
