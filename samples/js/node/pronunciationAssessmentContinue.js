// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as filePushStream from "./filePushStream.js";
import * as Segment from "segment";
import fs from 'fs';
import { v4 as uuidv4 } from 'uuid';
import { diffArrays } from "diff";
import _ from "lodash";

// pronunciation assessment with audio streaming and continue mode
export const main = (settings) => {

    // now create the audio-config pointing to our stream and
    // the speech config specifying the language.
    var wavFileHeader = filePushStream.readWavFileHeader(settings.filename);
    var format = sdk.AudioStreamFormat.getWaveFormatPCM(wavFileHeader.framerate, wavFileHeader.bitsPerSample, wavFileHeader.nChannels);
    var audioStream = filePushStream.openPushStream(settings.filename);
    var audioConfig = sdk.AudioConfig.fromStreamInput(audioStream, format);
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

    var reference_text = "What's the weather like?";
    var enableProsodyAssessment = true;
    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
        reference_text,
        sdk.PronunciationAssessmentGradingSystem.HundredMark,
        sdk.PronunciationAssessmentGranularity.Phoneme,
        true
    );
    pronunciationAssessmentConfig.enableProsodyAssessment = enableProsodyAssessment;

    // setting the recognition language to English.
    speechConfig.speechRecognitionLanguage = settings.language;

    // create the speech recognizer.
    var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    pronunciationAssessmentConfig.applyTo(reco);

    const scoreNumber = {
        accuracyScore: 0,
        fluencyScore: 0,
    };
    const allWords = [];
    var currentText = [];
    var startOffset = 0;
    var fluencyScores = [];
    var prosodyScores = [];
    var durations = [];
    var jo = {};
    var filePath = `${uuidv4()}.txt`;
    var recognizedWordsNum = 0;

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
        console.log(` Accuracy score: ${pronunciation_result.accuracyScore},` +
            `\n pronunciation score: ${pronunciation_result.pronunciationScore},` +
            `\n completeness score : ${pronunciation_result.completenessScore},` +
            `\n fluency score: ${pronunciation_result.fluencyScore},` +
            `${enableProsodyAssessment ? `\n prosody score: ${pronunciation_result.prosodyScore}` : ""}`
        );

        jo = JSON.parse(e.result.properties.getProperty(sdk.PropertyId.SpeechServiceResponse_JsonResult));
        const nb = jo["NBest"][0];
        startOffset = nb.Words[0].Offset;
        const localtext = _.map(nb.Words, (item) => item.Word.toLowerCase());
        currentText = currentText.concat(localtext);
        fluencyScores.push(nb.PronunciationAssessment.FluencyScore);
        prosodyScores.push(nb.PronunciationAssessment.ProsodyScore);
        const isSucceeded = jo.RecognitionStatus === 'Success';
        const nBestWords = jo.NBest[0].Words;
        const durationList = [];
        _.forEach(nBestWords, (word) => {
            if (word.PronunciationAssessment.AccuracyScore < 60 && word.PronunciationAssessment.ErrorType == "None") {
                word.PronunciationAssessment.ErrorType = "Mispronunciation";
            }
            if (word.PronunciationAssessment.ErrorType == "None") recognizedWordsNum++;
            durationList.push(word.Duration);
        });
        durations.push(_.sum(durationList));

        if (isSucceeded && nBestWords) {
            allWords.push(...nBestWords);
        }
    };

    function wordsToTempDict (words) {
        let data = "";
        const wordMap = {};
        for (const word of words) word in wordMap ? wordMap[word]++ : wordMap[word] = 1;
        for (const key in wordMap) data += key + `|0x00000000|${wordMap[key]}\n`;
        fs.writeFileSync(filePath, data.trim("\n"), (_e) => {});
    }

    function removeTempDict() {
        fs.unlink(filePath, (_e) => {});
    }

    function calculateOverallPronunciationScore() {
        let wholelyricsArray = [];

        // The sample code provides only zh-CN and en-US locales
        if (["zh-cn"].includes(settings.language.toLowerCase())) {
            wordsToTempDict(currentText);
            const wholelyrics = (reference_text.toLocaleLowerCase() ?? "").replace(new RegExp("[^a-zA-Z0-9\u4E00-\u9FA5']+", "g"), " ");
            const segment = new Segment.Segment();
            segment.use('DictTokenizer');
            segment.loadDict(filePath);
            _.map(segment.doSegment(wholelyrics, {stripPunctuation: true}), (res) => wholelyricsArray.push(res['w']));
            removeTempDict();
        } else {
            let wholelyrics = (reference_text.toLocaleLowerCase() ?? "").replace(new RegExp("[!\"#$%&()*+,-./:;<=>?@[^_`{|}~]+", "g"), "").replace(new RegExp("]+", "g"), "");
            wholelyricsArray = wholelyrics.split(" ");
        }
        const wholelyricsArrayRes = _.map(
            _.filter(wholelyricsArray, (item) => !!item),
            (item) => item.trim()
        );

        // For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
        // We need to compare with the reference text after received all recognized words to get these error words.
        var lastWords = [];
        if (reference_text.length != 0) {
            const diff = diffArrays(wholelyricsArrayRes, currentText);
            let currentWholelyricsArrayResIndex = 0;
            let currentResTextArrayIndex = 0;
            for (const d of diff) {
                if (d.added) {
                    _.map(allWords.slice(currentResTextArrayIndex, currentResTextArrayIndex + (d.count ?? 0)), (item) => {
                        if (item.PronunciationAssessment.ErrorType !== "Insertion") {
                            item.PronunciationAssessment.ErrorType = "Insertion";
                        }
                        lastWords.push(item);
                        currentResTextArrayIndex++;
                    });
                }
                if (d.removed) {
                    if (
                        currentWholelyricsArrayResIndex + (d.count ?? 0) + 1 == wholelyricsArrayRes.length &&
                        !(
                            jo.RecognitionStatus == "Success" ||
                            jo.RecognitionStatus == "Failed"
                        )
                    )
                    continue;
                    for (let i = 0; i < (d.count ?? 0); i++) {
                        const word = {
                            Word: wholelyricsArrayRes[currentWholelyricsArrayResIndex],
                            PronunciationAssessment: {
                                ErrorType: "Omission",
                            },
                        };
                        lastWords.push(word);
                        currentWholelyricsArrayResIndex++;
                    }
                }
                if (!d.added && !d.removed) {
                    _.map(allWords.slice(currentResTextArrayIndex, currentResTextArrayIndex + (d.count ?? 0)), (item) => {
                        lastWords.push(item);
                        currentWholelyricsArrayResIndex++;
                        currentResTextArrayIndex++;
                    });
                }
            }
        } else {
            lastWords = allWords;
        }

        if (reference_text.trim() != "") {
            let compScore =
                reference_text.length != 0
                    ? Number(((recognizedWordsNum / wholelyricsArrayRes.length) * 100).toFixed(0))
                    : 0;

            if (compScore > 100) {
                compScore = 100;
            }
            scoreNumber.compScore = compScore;
        }

        const accuracyScores = [];
        _.forEach(lastWords, (word) => {
            if (word && word?.PronunciationAssessment?.ErrorType != "Insertion") {
                accuracyScores.push(Number(word?.PronunciationAssessment.AccuracyScore ?? 0));
            }
        });
        scoreNumber.accuracyScore = Number((_.sum(accuracyScores) / accuracyScores.length).toFixed(0));

        if (startOffset) {
            const sumRes = [];
            _.forEach(fluencyScores, (x, index) => {
                sumRes.push(x * durations[index]);
            });
            scoreNumber.fluencyScore = _.sum(sumRes) / _.sum(durations);
        }

        enableProsodyAssessment && (scoreNumber.prosodyScore = _.sum(prosodyScores) / prosodyScores.length);

        const sortScore = Object.keys(scoreNumber).sort(function (a, b) {
            return scoreNumber[a] - scoreNumber[b];
        });
        if (reference_text.trim() != "" && enableProsodyAssessment) {
            scoreNumber.pronScore = Number(
                (
                    scoreNumber[sortScore["0"]] * 0.4 +
                    scoreNumber[sortScore["1"]] * 0.2 +
                    scoreNumber[sortScore["2"]] * 0.2 +
                    scoreNumber[sortScore["3"]] * 0.2
                ).toFixed(0)
            );
        } else if (reference_text.trim() != "" || enableProsodyAssessment) {
            scoreNumber.pronScore = Number(
                (scoreNumber[sortScore["0"]] * 0.6 + scoreNumber[sortScore["1"]] * 0.2 + scoreNumber[sortScore["2"]] * 0.2).toFixed(0)
            );
        } else {
            scoreNumber.pronScore = Number(
                (scoreNumber[sortScore["0"]] * 0.6 + scoreNumber[sortScore["1"]] * 0.4).toFixed(0)
            );
        }

        console.log(`    Paragraph pronunciation score: ${scoreNumber.pronScore},` +
            ` accuracy score: ${scoreNumber.accuracyScore},` +
            `${reference_text.trim() != "" ? ` completeness score: ${scoreNumber.compScore},` : ""}` +
            ` fluency score: ${scoreNumber.fluencyScore}` +
            `${enableProsodyAssessment ? `, prosody score: ${scoreNumber.prosodyScore}` : ""}`);

        _.forEach(lastWords, (word, ind) => {
            let wordLevelOutput = `     ${ind + 1}: word: ${word.Word}`;
            if (word.PronunciationAssessment.ErrorType != "Omission" && word.PronunciationAssessment.ErrorType != "Insertion") {
                wordLevelOutput += `\taccuracy score: ${word.PronunciationAssessment.AccuracyScore}\terror type: ${word.PronunciationAssessment.ErrorType};`;
            } else {
                wordLevelOutput += `\t\t\t\terror type: ${word.PronunciationAssessment.ErrorType};`;
            }
            console.log(wordLevelOutput);
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

    // Signals that a new session has started with the speech service
    reco.sessionStarted = function (s, e) {};

    // Signals the end of a session with the speech service.
    reco.sessionStopped = function (s, e) {
        reco.stopContinuousRecognitionAsync();
        reco.close();
        calculateOverallPronunciationScore();
    };

    reco.startContinuousRecognitionAsync();
}
