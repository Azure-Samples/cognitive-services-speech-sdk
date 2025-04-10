// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as filePushStream from "./filePushStream.js";
import * as difflib from "difflib";
import * as fs from "fs";
import _ from "lodash";

// pronunciation assessment with audio streaming and continue mode
export const main = async (settings) => {

    // now create the audio-config pointing to our stream and
    // the speech config specifying the language.
    var wavFileHeader = filePushStream.readWavFileHeader(settings.filename);
    var format = sdk.AudioStreamFormat.getWaveFormatPCM(wavFileHeader.framerate, wavFileHeader.bitsPerSample, wavFileHeader.nChannels);
    var audioStream = filePushStream.openPushStream(settings.filename);
    var audioConfig = sdk.AudioConfig.fromStreamInput(audioStream, format);
    var speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);

    // You can adjust the segmentation silence timeout based on your real scenario.
    speechConfig.setProperty(
        sdk.PropertyId.Speech_SegmentationSilenceTimeoutMs,
        "1500"
    );

    var reference_text = "What's the weather like?";
    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    const pronunciationAssessmentConfig = new sdk.PronunciationAssessmentConfig(
        reference_text,
        sdk.PronunciationAssessmentGradingSystem.HundredMark,
        sdk.PronunciationAssessmentGranularity.Phoneme,
        true
    );
    pronunciationAssessmentConfig.enableProsodyAssessment = true;

    // setting the recognition language to English.
    speechConfig.speechRecognitionLanguage = settings.language;

    // create the speech recognizer.
    var reco = new sdk.SpeechRecognizer(speechConfig, audioConfig);
    // (Optional) get the session ID
    reco.sessionStarted = (_s, e) => {
        console.log(`SESSION ID: ${e.sessionId}`);
    };
    pronunciationAssessmentConfig.applyTo(reco);

    const scoreNumber = {
        accuracyScore: 0,
        fluencyScore: 0,
        compScore: 0,
        prosodyScore: 0,
    };
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
            "prosody score: ", pronunciation_result.prosodyScore
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

    function convertReferenceWords(referenceText, referenceWords) {
        const dictionary = [...new Set(referenceWords)];
        const maxLength = Math.max(...dictionary.map(word => word.length));

        // From left to right to do the longest matching to get the word segmentation
        function leftToRightSegmentation(text, dictionary) {
            var result = [];
            while (text.length > 0) {
                let subText = "";
                // If the length of the text is less than the maxLength, then the subText is the text itself
                if (text.length < maxLength) {
                    subText = text;
                } else {
                    subText = text.substring(0, maxLength);
                }
                while (subText.length > 0) {
                    // If the subText is in the dictionary or the length of the subText is 1, then add it to the result
                    if (dictionary.includes(subText) || subText.length === 1) {
                        result.push(subText);
                        // Remove the subText from the text
                        text = text.slice(subText.length);
                        break;
                    } else {
                        // If the subText is not in the dictionary, then remove the last character of the subText
                        subText = subText.slice(0, -1);
                    }
                }
            }
            return result;
        }

        // From right to left to do the longest matching to get the word segmentation
        function rightToLeftSegmentation(text, dictionary) {
            var result = [];
            while (text.length > 0) {
                let subText = "";
                // If the length of the text is less than the maxLength, then the subText is the text itself
                if (text.length < maxLength) {
                    subText = text;
                } else {
                    subText = text.slice(-maxLength);
                }
                while (subText.length > 0) {
                    // If the subText is in the dictionary or the length of the subText is 1, then add it to the result
                    if (dictionary.includes(subText) || subText.length === 1) {
                        result.push(subText);
                        // Remove the subText from the text
                        text = text.slice(0, -subText.length);
                        break
                    } else {
                        // If the subText is not in the dictionary, then remove the first character of the subText
                        subText = subText.slice(1);
                    }
                }
            }
            // Reverse the result to get the correct order
            result = result.reverse();
            return result;
        }

        function segment_word(referenceText, dictionary) {
            const leftToRight = leftToRightSegmentation(referenceText, dictionary);
            const rightToLeft = rightToLeftSegmentation(referenceText, dictionary);
            if (leftToRight.join("") === referenceText) {
                return leftToRight;
            } else if (rightToLeft.join("") === referenceText) {
                return rightToLeft;
            } else {
                console.log("WW failed to segment the text with the dictionary")
                if (leftToRight.length < rightToLeft.length) {
                    return leftToRight;
                } else if (leftToRight.length > rightToLeft.length) {
                    return rightToLeft;
                } else {
                    // If the word number is the same, then return the one with the smallest single word
                    const leftToRightSingle = leftToRight.filter(word => word.length === 1).length;
                    const rightToLeftSingle = rightToLeft.filter(word => word.length === 1).length;
                    if (leftToRightSingle < rightToLeftSingle) {
                        return leftToRight;
                    } else {
                        return rightToLeft;
                    }
                }
            }
        }

        // Remove punctuation from the reference text
        referenceText = referenceText.split("").filter(char => /[\p{L}\p{N}\s]/u.test(char)).join("");
        return segment_word(referenceText, dictionary);
    }

    async function getReferenceWords(waveFilename, referenceText, language) {
        const audioConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(waveFilename));
        const speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);
        speechConfig.speechRecognitionLanguage = language;

        const speechRecognizer = new sdk.SpeechRecognizer(speechConfig, audioConfig);

        // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
        const enableMiscue = true;
        const pronunciationConfig = new sdk.PronunciationAssessmentConfig(
            referenceText,
            sdk.PronunciationAssessmentGradingSystem.HundredMark,
            sdk.PronunciationAssessmentGranularity.Phoneme,
            enableMiscue
        );

        // Apply pronunciation assessment config to speech recognizer
        pronunciationConfig.applyTo(speechRecognizer);
        const res = await new Promise((resolve, reject) => {
            speechRecognizer.recognizeOnceAsync(
                (result) => {
                    const referenceWords = [];
                    if (result.reason == sdk.ResultReason.RecognizedSpeech) {
                        const jo = JSON.parse(result.properties.getProperty(sdk.PropertyId.SpeechServiceResponse_JsonResult));
                        _.forEach(jo.NBest[0].Words, (word) => {
                            if (word.PronunciationAssessment.ErrorType != "Insertion") {
                                referenceWords.push(word.Word.toLowerCase());
                            }
                        })
                    } else if (result.reason == sdk.ResultReason.NoMatch) {
                        console.log("No speech could be recognized");
                        reject([]);
                    } else if (result.reason == sdk.ResultReason.Canceled) {
                        console.log(`Speech Recognition canceled: ${result.errorDetails}`);
                        reject([]);
                    }
                    resolve(convertReferenceWords(referenceText, referenceWords));
                    speechRecognizer.close();
                },
                (err) => {
                    reject(err);
                    speechRecognizer.close();
                }
            );
        });

        return res
    }

    async function calculateOverallPronunciationScore() {
        let referenceWords = [];

        // The sample code provides only zh-CN and en-US locales
        if (["zh-cn"].includes(settings.language.toLowerCase())) {
            // Split words for Chinese using the reference text and any short wave file
            referenceWords = await getReferenceWords(settings.dummyFilename, reference_text, settings.language);
        } else {
            const referenceText = reference_text.toLocaleLowerCase() ?? "";
            referenceWords = _.map(
                _.filter(referenceText.split(" "), (item) => !!item),
                (item) => item.replace(/^[\s!\"#$%&()*+,-./:;<=>?@[\]^_`{|}~]+|[\s!\"#$%&()*+,-./:;<=>?@[\]^_`{|}~]+$/g, "")
            );
        }

        // For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
        // We need to compare with the reference text after received all recognized words to get these error words.
        const diff = new difflib.SequenceMatcher(null, referenceWords, recognizedWordStrList);
        const lastWords = [];
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

        // Calculate whole completeness score
        let compScore = handledLastWords.length > 0 ? Number(((validWordCount / handledLastWords.length) * 100).toFixed(2)) : 0;
        scoreNumber.compScore = compScore > 100 ? 100 : compScore;

        // We can calculate whole accuracy by averaging
        scoreNumber.accuracyScore = Number((_.sum(accuracyScores) / accuracyScores.length).toFixed(2));

        // Re-calculate fluency score
        if (startOffset > 0) {
            scoreNumber.fluencyScore = Number((_.sum(durations) / (endOffset - startOffset) * 100).toFixed(2));
        }

        // Re-calculate the prosody score by averaging
        scoreNumber.prosodyScore = Number((_.sum(prosodyScores) / prosodyScores.length).toFixed(2));

        const sortScore = Object.keys(scoreNumber).sort(function (a, b) {
            return scoreNumber[a] - scoreNumber[b];
        });

        scoreNumber.pronScore = Number(
            (
                scoreNumber[sortScore["0"]] * 0.4 +
                scoreNumber[sortScore["1"]] * 0.2 +
                scoreNumber[sortScore["2"]] * 0.2 +
                scoreNumber[sortScore["3"]] * 0.2
            ).toFixed(2)
        );

        console.log(
            "    Paragraph pronunciation score: ", scoreNumber.pronScore,
            ", accuracy score: ", scoreNumber.accuracyScore,
            ", completeness score: ", scoreNumber.compScore,
            ", fluency score: ", scoreNumber.fluencyScore,
            ", prosody score: ", scoreNumber.prosodyScore
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
