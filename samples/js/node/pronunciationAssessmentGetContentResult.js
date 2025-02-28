// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
// Please install the required packages from https://github.com/microsoft/cognitive-services-speech-sdk-js
import * as sdk from 'microsoft-cognitiveservices-speech-sdk';
import fs from 'fs';
import _ from 'lodash';

// content assessment with audio file
export const main = (settings) => {
    var audioConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(settings.filename));
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

    var url = `https://${settings.aoaiResourceName}.openai.azure.com/openai/deployments/${settings.aoaiDeploymentName}/` +
        `chat/completions?api-version=${settings.aoaiApiVersion}`;
    var headers = {"Content-Type": "application/json", "api-key": settings.aoaiApiKey};

    // setting the recognition language to English.
    speechConfig.speechRecognitionLanguage = "en-US";

    const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
        "",
        sdk.PronunciationAssessmentGradingSystem.HundredMark,
        sdk.PronunciationAssessmentGranularity.Phoneme,
        false
    );
    pronunciationAssessmentConfig.enableProsodyAssessment = true;

    // create the speech recognizer.
    var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    // (Optional) get the session ID
    reco.sessionStarted = (_s, e) => {
        console.log(`SESSION ID: ${e.sessionId}`);
    };
    pronunciationAssessmentConfig.applyTo(reco);

    const recognizedTexts = [];

    reco.recognized = function (s, e) {
        var jo = JSON.parse(e.result.properties.getProperty(sdk.PropertyId.SpeechServiceResponse_JsonResult));
        console.log(`Recognizing: ${jo.DisplayText}`);
        recognizedTexts.push(jo.DisplayText);
    }

    async function getContentScores(recognizedText, topic) {
        const data = {
            "messages": [
                {
                    "role": "system",
                    "content": `You are an English teacher and please help to grade a student's essay from vocabulary and grammar and topic relevance on how well the essay aligns with the title, and output format as: {"vocabularyScore": *.*(0-10), "grammarScore": *.*(0-10), "topicScore": *.*(0-10)}.`
                },
                {
                    "role": "user",
                    // Please provide your own sample sentences along with their scores.
                    "content": `Example1: this essay: '{sampleSentence1}' has vocabulary and grammar scores of * and *, respectively. ` +
                        `Example2: this essay: '{sampleSentence2}' has vocabulary and grammar scores of * and *, respectively. ` +
                        `Example3: this essay: '{sampleSentence3}' has vocabulary and grammar scores of * and *, respectively. ` +
                        `The essay for you to score is '${recognizedText}', and the title is '${topic}'. ` +
                        `The script is from speech recognition so that please first add punctuations when needed, remove duplicates and unnecessary un uh from oral speech, then find all the misuse of words and grammar errors in this essay, find advanced words and grammar usages, and finally give scores based on this information. Please only response as this format {"vocabularyScore": *.*(0-10), "grammarScore": *.*(0-10), "topicScore": *.*(0-10)}`
                }
            ],
            "temperature": 0,
            "top_p": 1
        }

        const response = await fetch(url, {
            method: "POST",
            headers: headers,
            body: JSON.stringify(data)
        });
        
        const result = await response.json();
        const content = JSON.parse(result.choices[0].message.content);

        return content;
    }

    function onRecognizedResult() {
        const recognizedText = recognizedTexts.join(" ");
        console.log(`Recognized text: ${recognizedText}`);
        getContentScores(recognizedText, settings.topic).then((contentResult) => {
            console.log("Content assessment result: \n",
                "\tvocabulary score: ", Number(contentResult.vocabularyScore.toFixed(1)), '\n',
                "\tgrammar score: ", Number(contentResult.grammarScore.toFixed(1)), '\n',
                "\ttopic score: ", Number(contentResult.topicScore.toFixed(1))
            );
        });
    }

    reco.canceled = function (s, e) {
        if (e.reason === sdk.CancellationReason.Error) {
            var str = "(cancel) Reason: " + sdk.CancellationReason[e.reason] + ": " + e.errorDetails;
            console.log(str);
        }
        reco.stopContinuousRecognitionAsync();
    };

    reco.sessionStopped = function (s, e) {
        reco.stopContinuousRecognitionAsync();
        reco.close();
        onRecognizedResult();
    };

    reco.startContinuousRecognitionAsync();
}
