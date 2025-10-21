// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import fs from "fs";
import { v4 as uuidv4 } from "uuid";

// pronunciation assessment with REST API.
export const main = async (settings) => {

    // Build pronunciation assessment parameters
    const audioStream = fs.createReadStream(settings.filename);
    const referenceText = `What's the weather like?`;
    const enableProsodyAssessment = true;
    const phonemeAlphabet = "SAPI"; // IPA or SAPI
    const enableMiscue = true;
    const nbestPhonemeCount = 5;

    const pronAssessmentParamsJson = JSON.stringify({
        GradingSystem: "HundredMark",
        Dimension: "Comprehensive",
        ReferenceText: referenceText,
        EnableProsodyAssessment: enableProsodyAssessment,
        PhonemeAlphabet: phonemeAlphabet,
        EnableMiscue: enableMiscue,
        NBestPhonemeCount: nbestPhonemeCount
    });

    const pronAssessmentParamsBase64 = Buffer.from(pronAssessmentParamsJson, "utf-8").toString("base64");
    const pronAssessmentParams = pronAssessmentParamsBase64;

    // https://learn.microsoft.com/azure/ai-services/speech-service/how-to-get-speech-session-id#provide-session-id-using-rest-api-for-short-audio
    const sessionId = uuidv4().replace(/-/g, "");

    let url = `https://${settings.serviceRegion}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1`;
    url = `${url}?format=detailed&language=${settings.language}&X-ConnectionId=${sessionId}`;

    const headers = {
        "Accept": "application/json;text/xml",
        "Connection": "Keep-Alive",
        "Content-Type": "audio/wav; codecs=audio/pcm; samplerate=16000",
        "Ocp-Apim-Subscription-Key": settings.subscriptionKey,
        "Pronunciation-Assessment": pronAssessmentParams,
    };

    console.log(`II URL: ${url}`);
    console.log(`II Config: ${pronAssessmentParamsJson}`);

    const start = Date.now();
    const response = await fetch(url, {
        method: "POST",
        headers,
        body: audioStream,
        duplex: "half",
    });
    const getResponseTime = Date.now();
    console.log("Response Time:", (getResponseTime - start) / 1000, "s");

    // Show Session ID
    console.log(`II Session ID: ${sessionId}`);

    if (!response.ok) {
        console.error(`EE Error code: ${response.status}`);
        console.error(`EE Error message: ${await response.text()}`);
        process.exit(1);
    } else {
        const data = await response.json();
        console.log("II Response:", data);
    }
}
