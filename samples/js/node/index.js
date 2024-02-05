// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as settings from "./settings.js";
import * as speech from "./speech.js";
import * as intent from "./intent.js";
import * as translate from "./translation.js";
import * as synthesis from "./synthesis.js";
import * as pronunciationAssessmentContinue from "./pronunciationAssessmentContinue.js";
import * as pronunciationAssessment from "./pronunciationAssessment.js";
import * as pronunciationAssessmentConfiguredWithJson from "./pronunciationAssessmentConfiguredWithJson.js";
import * as pronunciationAssessmentFromMicrophone from "./pronunciationAssessmentFromMicrophone.js";
import * as pronunciationAssessmentGetContentResult from "./pronunciationAssessmentGetContentResult.js";

if (process.argv.length > 3) {
    settings.filename = process.argv[3];
}

if (process.argv.length > 2) {
    switch (process.argv[2]) {
        case "intent":
            console.log("Now recognizing intent from: " + settings.filename);
            intent.main(settings);
            break;

        case "translate":
            console.log("Now translating from: " + settings.filename);
            translate.main(settings);
            break;

        case "synthesis":
            console.log("Now synthesizing to: " + settings.filename);
            synthesis.main(settings, settings.filename);
            break;
        
        case "pronunciationAssessmentContinue":
            console.log("Now pronunciation assessment to: " + settings.filename);
            pronunciationAssessmentContinue.main(settings);
            break;

        case "pronunciationAssessment":
            console.log("Now pronunciation assessment to: " + settings.filename);
            pronunciationAssessment.main(settings);
            break;

        case "pronunciationAssessmentConfiguredWithJson":
            console.log("Now pronunciation assessment to: " + settings.filename);
            pronunciationAssessmentConfiguredWithJson.main(settings);
            break;

        case "pronunciationAssessmentFromMicrophone":
            pronunciationAssessmentFromMicrophone.main(settings);
            break;

        case "pronunciationAssessmentWithContentAssessment":
            pronunciationAssessmentGetContentResult.main(settings);
            break;

        case "speech":
        default:
            console.log("Now recognizing speech from: " + settings.filename);
            speech.main(settings);
            break;
    }
}
else {
    console.log("usage: index.js [speech|intent|translate|synthesis|pronunciationAssessment|pronunciationAssessmentContinue|pronunciationAssessmentConfiguredWithJson|pronunciationAssessmentFromMicrophone] {filename}");
}
