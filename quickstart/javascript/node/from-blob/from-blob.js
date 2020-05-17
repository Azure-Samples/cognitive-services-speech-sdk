
(function () {
    "use strict";
    var https = require("https");

    // Replace with your subscription key
    SubscriptionKey = "YourSubscriptionKey";

    // Update with your service region
    Region = "YourServiceRegion";
    Port = 443;

    // Recordings and locale
    Locale = "en-US";
    RecordingsBlobUri = "`YourAudioFile.wav";

    // Name and description
    Name = "Simple transcription";
    Description = "Simple transcription description";

    SpeechToTextBasePath = "/api/speechtotext/v2.0/";

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
        Name: Name,
        Description: Description,
        Locale: Locale,
        RecordingsUrl: RecordingsBlobUri,
        Properties: {
            "PunctuationMode": "DictatedAndAutomatic",
            "ProfanityFilterMode": "Masked",
            "AddWordLevelTimestamps": "True"
        },
        Models: []
    };

    var postPayload = JSON.stringify(ts);

    var startOptions = {
        hostname: Region + ".cris.ai",
        port: Port,
        path: SpeechToTextBasePath + "Transcriptions/",
        method: "POST",
        headers: {
            "Content-Type": "application/json",
            'Content-Length': postPayload.length,
            "Ocp-Apim-Subscription-Key": SubscriptionKey
        }
    }

    function PrintResults(resultUrl) {
        var fetchOptions = {
            headers: {
                "Ocp-Apim-Subscription-Key": SubscriptionKey
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

    function CheckTranscriptionStatus(statusUrl) {
        transcription = null;
        var fetchOptions = {
            headers: {
                "Ocp-Apim-Subscription-Key": SubscriptionKey
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
                            console.info("Transcription failed. Status: " + transcription.StatusMessage);
                            done = true;
                            break;
                        case "Succeeded":
                            done = true;
                            PrintResults(statusObject.resultsUrls["channel_0"]);
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
                            CheckTranscriptionStatus(statusUrl);
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
        if (response.statusCode != 202) {
            console.error("Error, status code " + response.statusCode);
        } else {

            var transcriptionLocation = response.headers.location;

            console.info("Created transcription at location " + transcriptionLocation);
            console.info("Checking status.");

            CheckTranscriptionStatus(transcriptionLocation);
        }
    });

    request.on("error", function (error) {
        console.error(error);
    });

    request.write(postPayload);
    request.end();

}());
