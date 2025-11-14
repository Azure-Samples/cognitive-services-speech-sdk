// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as settings from "./settings.js";
import * as speech from "./speech.js";
import * as speechTokenCredential from "./speechTokenCredential.js";
import * as speechKeyCredential from "./speechKeyCredential.js";
import * as translate from "./translation.js";
import * as translationTokenCredential from "./translationTokenCredential.js";
import * as translationKeyCredential from "./translationKeyCredential.js";
import * as synthesis from "./synthesis.js";

if (process.argv.length > 3) {
    settings.filename = process.argv[3];
}

if (process.argv.length > 2) {
    switch (process.argv[2]) {
        case "translate":
            console.log("Now translating from: " + settings.filename);
            translate.main(settings);
            break;

        case "translationTokenCredential":
            console.log("Now translating with token credential from: " + settings.filename);
            translationTokenCredential.main(settings);
            break;

        case "translationKeyCredential":
            console.log("Now translating with key credential from: " + settings.filename);
            translationKeyCredential.main(settings);
            break;

        case "synthesis":
            console.log("Now synthesizing to: " + settings.filename);
            synthesis.main(settings, settings.filename);
            break;

        case "speechTokenCredential":
            console.log("Now recognizing speech with token credential from: " + settings.filename);
            speechTokenCredential.main(settings);
            break;

        case "speechKeyCredential":
            console.log("Now recognizing speech with key credential from: " + settings.filename);
            speechKeyCredential.main(settings);
            break;

        case "speech":
        default:
            console.log("Now recognizing speech from: " + settings.filename);
            speech.main(settings);
            break;
    }
}
else {
    console.log("usage: index.js [speech|speechTokenCredential|speechKeyCredential|translate|translationTokenCredential|translationKeyCredential|synthesis] {filename}");
}
