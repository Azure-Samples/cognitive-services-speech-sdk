// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as utils from "./utils.js";
import * as difflib from "difflib";
import _ from "lodash";

// pronunciation assessment with audio streaming and continue mode
export const main = async (settings) => {

    // now create the audio-config pointing to our stream and
    // the speech config specifying the language.
    var wavFileHeader = utils.readWavFileHeader(settings.filename);
    var format = sdk.AudioStreamFormat.getWaveFormatPCM(wavFileHeader.framerate, wavFileHeader.bitsPerSample, wavFileHeader.nChannels);
    var audioStream = utils.openPushStream(settings.filename);
    var audioConfig = sdk.AudioConfig.fromStreamInput(audioStream, format);
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

    // You can adjust the segmentation silence timeout based on your real scenario.
    speechConfig.setProperty(
        sdk.PropertyId.Speech_SegmentationSilenceTimeoutMs,
        "1500"
    );

    var referenceText = "What's the weather like?";
    var enableMiscue = true;
    var enableProsodyAssessment = true;
    var unscriptedScenario = referenceText.length > 0 ? false : true;

    var referenceWords = [];

    // We need to convert the reference text to lower case, and split to words, then remove the punctuations.
    if (["zh-cn"].includes(settings.language.toLowerCase())) {
        // Word segmentation for Chinese using the reference text and any short wave file
        // Remove the blank characters in the reference text
        referenceText = referenceText.replace(/ /g, "");
        referenceWords = await utils.getReferenceWords(settings, referenceText);
    } else {
        referenceText = referenceText.toLocaleLowerCase() ?? "";
        referenceWords = _.map(
            _.filter(referenceText.split(" "), (item) => !!item),
            (item) => item.replace(/^[\s!\"#$%&()*+,-./:;<=>?@[\]^_`{|}~]+|[\s!\"#$%&()*+,-./:;<=>?@[\]^_`{|}~]+$/g, "")
        );
    }
    // Remove empty words
    referenceWords = referenceWords.filter(w => w.trim().length > 0);
    referenceText = referenceWords.join(" ");
    console.log("Reference text:", referenceText);

    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
        referenceText,
        sdk.PronunciationAssessmentGradingSystem.HundredMark,
        sdk.PronunciationAssessmentGranularity.Phoneme,
        enableMiscue
    );
    pronunciationAssessmentConfig.enableProsodyAssessment = enableProsodyAssessment;

    speechConfig.speechRecognitionLanguage = settings.language;

    // Creates a speech recognizer using a file as audio input.
    var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    // (Optional) get the session ID
    reco.sessionStarted = (_s, e) => {
        console.log(`SESSION ID: ${e.sessionId}`);
    };
    pronunciationAssessmentConfig.applyTo(reco);

    const allWords = [];
    var recognizedWordStrList = [];
    var startOffset = 0;
    var endOffset = 0;
    var prosodyScores = [];
    var durations = [];
    var jo = {};

    // Before beginning speech recognition, setup the callbacks to be invoked when an event occurs.

    // The event recognizing signals that an intermediate recognition result is received.
    // You will receive one or more recognizing events as a speech phrase is recognized, with each containing
    // more recognized speech. The event will contain the text for the recognition since the last phrase was recognized.
    reco.recognizing = function (s, e) {
        var str = "(recognizing) Reason: " + sdk.ResultReason[e.result.reason] + " Text: " + e.result.text;
        console.log(str);
    };

    // The event recognized signals that a final recognition result is received.
    // This is the final event that a phrase has been recognized.
    // For continuous recognition, you will get one recognized event for each phrase recognized.
    reco.recognized = function (s, e) {
        console.log("pronunciation assessment for: ", e.result.text);
        var pronunciation_result = sdk.PronunciationAssessmentResult.fromResult(e.result);
        console.log(" Accuracy score: ", pronunciation_result.accuracyScore, '\n',
            "pronunciation score: ", pronunciation_result.pronunciationScore, '\n',
            "completeness score : ", pronunciation_result.completenessScore, '\n',
            "fluency score: ", pronunciation_result.fluencyScore, '\n',
            "prosody score: ", pronunciation_result.prosodyScore ?? NaN
        );

        jo = JSON.parse(e.result.properties.getProperty(sdk.PropertyId.SpeechServiceResponse_JsonResult));
        const nb = jo["NBest"][0];
        const localtext = _.map(nb.Words, (item) => item.Word.toLowerCase());
        recognizedWordStrList = recognizedWordStrList.concat(localtext);
        prosodyScores.push(nb.PronunciationAssessment.ProsodyScore);
        const isSucceeded = jo.RecognitionStatus === 'Success';
        const nBestWords = jo.NBest[0].Words;

        if (isSucceeded && nBestWords) {
            allWords.push(...nBestWords);
        }

        if (startOffset == 0) {
            startOffset = nb.Words[0].Offset;
        }
        endOffset = nb.Words.slice(-1)[0].Offset + nb.Words.slice(-1)[0].Duration + 100000;
    };

    async function calculateOverallPronunciationScore() {
        let lastWords = [];
        if (enableMiscue && !unscriptedScenario) {
            // align the reference words basing on recognized words.
            referenceWords = utils.alignListsWithDiffHandling(
                referenceWords,
                allWords.map(x => x.Word.toLowerCase())
            );
            // For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
            // We need to compare with the reference text after received all recognized words to get these error words.
            const diff = new difflib.SequenceMatcher(null, referenceWords, recognizedWordStrList);
            for (const d of diff.getOpcodes()) {
                if (d[0] == "insert" || d[0] == "replace") {
                    for (let j = d[3]; j < d[4]; j++) {
                        if (allWords && allWords.length > 0 && allWords[j].PronunciationAssessment.ErrorType !== "Insertion") {
                            allWords[j].PronunciationAssessment.ErrorType = "Insertion";
                        }
                        lastWords.push(allWords[j]);
                    }
                }
                if (d[0] == "delete" || d[0] == "replace") {
                    if (
                        d[2] == referenceWords.length &&
                        !(
                            jo.RecognitionStatus == "Success" ||
                            jo.RecognitionStatus == "Failed"
                        )
                    )
                    continue;
                    for (let i = d[1]; i < d[2]; i++) {
                        const word = {
                            Word: referenceWords[i],
                            PronunciationAssessment: {
                                ErrorType: "Omission",
                            },
                        };
                        lastWords.push(word);
                    }
                }
                if (d[0] == "equal") {
                    for (let k = d[3], count = 0; k < d[4]; count++) {
                        lastWords.push(allWords[k]);
                        k++;
                    }
                }
            }
        } else {
            lastWords = allWords;
        }

        // If accuracy score is below 60, mark as mispronunciation
        _.forEach(lastWords, (word) => {
            if (word.PronunciationAssessment.AccuracyScore < 60 && word.PronunciationAssessment.ErrorType == "None") {
                word.PronunciationAssessment.ErrorType = "Mispronunciation";
            }
        });

        const accuracyScores = [];
        const handledLastWords = [];
        let validWordCount = 0;
        _.forEach(lastWords, (word) => {
            if (word && word.PronunciationAssessment.ErrorType != "Insertion") {
                accuracyScores.push(Number(word.PronunciationAssessment.AccuracyScore ?? 0));
                handledLastWords.push(word.Word);
            }
            if (word.PronunciationAssessment.ErrorType == "None" && (word.PronunciationAssessment.AccuracyScore ?? 0) >= 0) {
                validWordCount++;
                durations.push(Number(word.Duration) + 100000);
            }
        });

        // We can calculate whole accuracy by averaging
        const accuracyScore = Number((_.sum(accuracyScores) / accuracyScores.length).toFixed(2));

        // Re-calculate the prosody score by averaging
        let prosodyScore = NaN;
        if (prosodyScores.length !== 0) {
            prosodyScore = Number((_.sum(prosodyScores) / prosodyScores.length).toFixed(2));
        }

        // Re-calculate fluency score
        let fluencyScore = 0;
        if (startOffset > 0) {
            fluencyScore = Number((_.sum(durations) / (endOffset - startOffset) * 100).toFixed(2));
        }

        // Calculate whole completeness score
        let compScore = 100;
        if (!unscriptedScenario) {
            compScore = Number(((validWordCount / handledLastWords.length) * 100).toFixed(2));
            compScore = compScore > 100 ? 100 : compScore;
        }

        let pronunciationScore = 0;
        if (!unscriptedScenario) {
            // Scripted scenario
            if (enableProsodyAssessment && !Number.isNaN(prosodyScore)) {
                const sortedScores = [accuracyScore, prosodyScore, compScore, fluencyScore].sort((a, b) => a - b);
                pronunciationScore =
                    sortedScores[0] * 0.4 +
                    sortedScores[1] * 0.2 +
                    sortedScores[2] * 0.2 +
                    sortedScores[3] * 0.2;
            } else {
                const sortedScores = [accuracyScore, compScore, fluencyScore].sort((a, b) => a - b);
                pronunciationScore =
                    sortedScores[0] * 0.6 +
                    sortedScores[1] * 0.2 +
                    sortedScores[2] * 0.2;
            }
        } else {
            // Unscripted scenario
            if (enableProsodyAssessment && !Number.isNaN(prosodyScore)) {
                const sortedScores = [accuracyScore, prosodyScore, fluencyScore].sort((a, b) => a - b);
                pronunciationScore =
                    sortedScores[0] * 0.6 +
                    sortedScores[1] * 0.2 +
                    sortedScores[2] * 0.2;
            } else {
                const sortedScores = [accuracyScore, fluencyScore].sort((a, b) => a - b);
                pronunciationScore =
                    sortedScores[0] * 0.6 +
                    sortedScores[1] * 0.4;
            }
        }

        console.log(
            "    Paragraph pronunciation score: ", pronunciationScore.toFixed(0),
            ", accuracy score: ", accuracyScore.toFixed(0),
            ", completeness score: ", compScore.toFixed(0),
            ", fluency score: ", fluencyScore.toFixed(0),
            ", prosody score: ", prosodyScore.toFixed(0)
        );

        _.forEach(lastWords, (word, ind) => {
            console.log(
                "    ", ind + 1, ": word: ", word.Word,
                "\taccuracy score: ", word.PronunciationAssessment.AccuracyScore,
                "\terror type: ", word.PronunciationAssessment.ErrorType, ";"
            );
        });

    };

    // The event signals that the service has stopped processing speech.
    // https://docs.microsoft.com/javascript/api/microsoft-cognitiveservices-speech-sdk/speechrecognitioncanceledeventargs?view=azure-node-latest
    // This can happen for two broad classes of reasons.
    // 1. An error is encountered.
    //    In this case the .errorDetails property will contain a textual representation of the error.
    // 2. Speech was detected to have ended.
    //    This can be caused by the end of the specified file being reached, or ~20 seconds of silence from a microphone input.
    reco.canceled = function (s, e) {
        if (e.reason === sdk.CancellationReason.Error) {
            var str = "(cancel) Reason: " + sdk.CancellationReason[e.reason] + ": " + e.errorDetails;
            console.log(str);
        }
        reco.stopContinuousRecognitionAsync();
    };

    // Signals the end of a session with the speech service.
    reco.sessionStopped = function (s, e) {
        reco.stopContinuousRecognitionAsync();
        reco.close();
        calculateOverallPronunciationScore();
    };

    reco.startContinuousRecognitionAsync();
}
