// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";
import _ from "lodash";

function ReadInt32(fd) {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    if (4 != bytesRead) {
        throw "Error reading 32-bit integer from .wav file header. Expected 4 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readInt32LE();
}

function ReadUInt16(fd) {
    var buffer = Buffer.alloc(2);
    var bytesRead = fs.readSync(fd, buffer);
    if (2 != bytesRead) {
        throw "Error reading 16-bit unsigned integer from .wav file header. Expected 2 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readUInt16LE();
}

function ReadUInt32(fd) {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    if (4 != bytesRead) {
        throw "Error reading unsigned 32-bit integer from .wav file header. Expected 4 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readUInt32LE();
}

function ReadString(fd, length) {
    var buffer = Buffer.alloc(length);
    var bytesRead = fs.readSync(fd, buffer);
    if (length != bytesRead) {
        throw "Error reading string from .wav file header. Expected " + String(length) + " bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.toString();
}

export const openPushStream = (filename) => {

    // Get the wave header for the file.
    var wavFileHeader = readWavFileHeader(filename);

    var format;

    switch (wavFileHeader.tag)
    {
        case 1: // PCM
            format = sdk.AudioFormatTag.PCM;
            break;
        case 6: 
            format = sdk.AudioFormatTag.ALaw;
            break;
        case 7:
            format = sdk.AudioFormatTag.MuLaw;
            break;
        default:
            throw new Error("Wave format " + wavFileHeader.tag + " is not supported");
    }

    // Create the format for PCM Audio.
    var format = sdk.AudioStreamFormat.getWaveFormat(wavFileHeader.framerate, wavFileHeader.bitsPerSample, wavFileHeader.nChannels, format);

    // create the push stream we need for the speech sdk.
    var pushStream = sdk.AudioInputStream.createPushStream(format);

    // open the file and push it to the push stream. 
    // Notice: we skip 44 bytes for the typical wav header.
    fs.createReadStream(filename, { start: 44}).on('data', function(arrayBuffer) {
        pushStream.write(arrayBuffer.slice());
    }).on('end', function() {
        pushStream.close();
    });

    return pushStream;
};

export const readWavFileHeader = (audioFileName) => {
    var fd = fs.openSync(audioFileName, 'r');

    if(ReadString(fd, 4) != "RIFF") {
        throw "Error reading .wav file header. Expected 'RIFF' tag.";
    }
    // File length
    ReadInt32(fd);
    if(ReadString(fd, 4) != "WAVE") {
        throw "Error reading .wav file header. Expected 'WAVE' tag.";
    }
    if(ReadString(fd, 4) != "fmt ") {
        throw "Error reading .wav file header. Expected 'fmt ' tag.";
    }
    // Format size
    var formatSize = ReadInt32(fd);
    if (formatSize > 16) {
        throw "Error reading .wav file header. Expected format size 16 bytes. Actual size: " + String(formatSize);
    }
    // Format tag
    var tag = ReadUInt16(fd);
    var nChannels = ReadUInt16(fd);
    var framerate = ReadUInt32(fd);
    // Average bytes per second
    ReadUInt32(fd);
    // Block align
    ReadUInt16(fd);
    var bitsPerSample = ReadUInt16(fd);

    fs.closeSync(fd);

    return { framerate : framerate, bitsPerSample : bitsPerSample, nChannels : nChannels, tag: tag };
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

export const getReferenceWords = async (settings, referenceText) => {
    const audioConfig = sdk.AudioConfig.fromWavFileInput(fs.readFileSync(settings.dummyFilename));
    const speechConfig = sdk.SpeechConfig.fromSubscription(settings.subscriptionKey, settings.serviceRegion);
    speechConfig.speechRecognitionLanguage = settings.language;

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
                    reject("No speech could be recognized");
                } else if (result.reason == sdk.ResultReason.Canceled) {
                    reject(`Speech Recognition canceled: ${result.errorDetails}`);
                }
                resolve(convertReferenceWords(referenceText, referenceWords));
                speechRecognizer.close();
            },
            (err) => {
                reject(err);
                speechRecognizer.close();
            }
        );
    }).catch((reason) => console.log(reason));

    return res
}