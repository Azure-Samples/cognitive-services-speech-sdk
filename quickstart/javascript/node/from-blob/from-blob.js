
(function () {
    "use strict";
    var https = require("https");

    // Replace with your subscription key
    var subscriptionKey = "YourSubscriptionKey";

    // Update with your service region
    var region = "YourServiceRegion";
    var port = 443;

    // Recordings and locale
    var locale = "en-US";
    var recordingsBlobUri = "`YourAudioFile.wav";

    // Name and description
    var name = "Simple transcription";
    var description = "Simple transcription description";

    var speechToTextBasePath = "/api/speechtotext/v2.0/";

    // These classes show the properties on JSON objects returned by the Speech Service or sent to it.
    /*
    class ModelIdentity {
        id;
    }
    
    class Transcription {
        Name;
        Description;
        Locale;
        RecordingsUrl;
        ResultsUrls;
        Id;
        CreatedDateTime;
        LastActionDateTime;
        Status;
        StatusMessage;
    }
    
    class TranscriptionDefinition {
        Name;
        Description;
        RecordingsUrl;
        Locale;
        Models;
        Properties;
    }
    */

    var ts = {
        Name: name,
        Description: description,
        Locale: locale,
        RecordingsUrl: recordingsBlobUri,
        Properties: {
            "PunctuationMode": "DictatedAndAutomatic",
            "ProfanityFilterMode": "Masked",
            "AddWordLevelTimestamps": "True"
        },
        Models: []
    };

    var postPayload = JSON.stringify(ts);

    var startOptions = {
        hostname: region + ".cris.ai",
        port: port,
        path: speechToTextBasePath + "Transcriptions/",
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            'Content-Length': postPayload.length,
            "Ocp-Apim-Subscription-Key": subscriptionKey
        }
    }

    function printResults(resultUrl) {
        var fetchOptions = {
            headers: {
                "Ocp-Apim-Subscription-Key": subscriptionKey
            }
        }

        var fetchRequest = https.get(new URL(resultUrl), fetchOptions, function (response) {
            if (response.statusCode !== 200) {
                console.info("Error retrieving status: " + response.statusCode);
            } else {
                var responseText = '';
                response.setEncoding('utf8');
                response.on("data", function (chunk) {
                    responseText += chunk;
                });

                response.on("end", function () {
                    console.info("Transcription Results:");
                    console.info(responseText);
                });
            }
        });
    }

    function checkTranscriptionStatus(statusUrl) {
        var fetchOptions = {
            headers: {
                "Ocp-Apim-Subscription-Key": subscriptionKey
            }
        }

        var fetchRequest = https.get(new URL(statusUrl), fetchOptions, function (response) {
            if (response.statusCode !== 200) {
                console.info("Error retrieving status: " + response.statusCode);
            } else {
                var responseText = '';
                response.setEncoding('utf8');
                response.on("data", function (chunk) {
                    responseText += chunk;
                });

                response.on("end", function () {
                    var statusObject = JSON.parse(responseText);
                    var done = false;
                    switch (statusObject.status) {
                        case "Failed":
                            console.info("Transcription failed. Status: " + statusObject.statusMessage);
                            done = true;
                            break;
                        case "Succeeded":
                            done = true;
                            printResults(statusObject.resultsUrls["channel_0"]);
                            break;
                        case "Running":
                            console.info("Transcription is still running.");
                            break;
                        case "NotStarted":
                            console.info("Transcription has not started.");
                            break;
                    }

                    if (!done) {
                        setTimeout(function () {
                            checkTranscriptionStatus(statusUrl);
                        }, (5000));
                    }
                });
            }
        });

        fetchRequest.on("error", function (error) {
            console.error(error);
        });
    }

    var request = https.request(startOptions, function (response) {
        if (response.statusCode !== 202) {
            console.error("Error, status code " + response.statusCode);
        } else {

            var transcriptionLocation = response.headers.location;

            console.info("Created transcription at location " + transcriptionLocation);
            console.info("Checking status.");

            checkTranscriptionStatus(transcriptionLocation);
        }
    });

    request.on("error", function (error) {
        console.error(error);
    });

    request.write(postPayload);
    request.end();

}());
