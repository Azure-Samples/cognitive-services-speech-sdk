// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as MicToSpeech from "mic-to-speech";
import * as wav from "wav";
import * as _ from "lodash";
import * as readline from "readline";
import { exit } from "process";


// pronunciation assessment with audio file
export const main = (settings) => {
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);
    // setting the recognition language to English.
    speechConfig.speechRecognitionLanguage = settings.language;

    function onRecognizedResult(result, reco) {
        var pronunciation_result = sdk.PronunciationAssessmentResult.fromResult(result);
        console.log(" Accuracy score: ", pronunciation_result.accuracyScore, '\n',
            "pronunciation score: ", pronunciation_result.pronunciationScore, '\n',
            "completeness score : ", pronunciation_result.completenessScore, '\n',
            "fluency score: ", pronunciation_result.fluencyScore
        );
        console.log("  Word-level details:");
        _.forEach(pronunciation_result.detailResult.Words, (word, idx) => {
            console.log("    ", idx + 1, ": word: ", word.Word, "\taccuracy score: ", word.PronunciationAssessment.AccuracyScore, "\terror type: ", word.PronunciationAssessment.ErrorType, ";");
        });
        reco.close();
        exit();
    }

    function recognizeFromMic(buffer, reference_text) {
        /** Writing WAV Headers into the buffer received. */
        let writer = new wav.Writer({
            sampleRate: 32000,
            channels: 1,
            bitDepth: 16,
        });
        writer.write(buffer);
        writer.end();
        writer.on("finish", ()=>{
            /** On finish, read the WAV stream using configuration the SDK provides. */
            let audioConfig = sdk.AudioConfig.fromWavFileInput(writer.read());

            // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
            const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
                reference_text,
                sdk.PronunciationAssessmentGradingSystem.HundredMark,
                sdk.PronunciationAssessmentGranularity.Phoneme,
                true
            );

            // create the speech recognizer.
            var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
            pronunciationAssessmentConfig.applyTo(reco);

            reco.recognizeOnceAsync(
                function (successfulResult) {
                    onRecognizedResult(successfulResult, reco);
                },
                function (err) {
                    console.log("ERROR: " + err);
                    exit();
                }
            );
        });
    }

    try {
        const inputProcess = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
        inputProcess.question("Enter reference text you want to assess, or enter empty text to exit. \n> ", (inputText) => {
            if (!inputText) {
                exit();
            } else {
                /** Configuration for the mic */
                let micToSpeech = new MicToSpeech({
                    channels: 1,
                });
                /** Receive the audio buffer from mic */
                micToSpeech.on("speech", function (buffer) {
                    console.log("buffer of speech received");
                    recognizeFromMic(buffer, inputText);
                });
                /** Start listening to speech. */
                micToSpeech.start();
                console.log("Listening for speech");
            }
            inputProcess.close();
        });
    } catch (e) {
        console.log("ERROR:", e);
        exit();
    }
}
