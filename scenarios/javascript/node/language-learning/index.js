// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as settings from "./settings.js";
import * as pronunciationAssessmentContinue from "./pronunciationAssessmentContinue.js";
import * as pronunciationAssessment from "./pronunciationAssessment.js";
import * as pronunciationAssessmentConfiguredWithJson from "./pronunciationAssessmentConfiguredWithJson.js";
import * as pronunciationAssessmentFromMicrophone from "./pronunciationAssessmentFromMicrophone.js";
import * as pronunciationAssessmentWithRestAPI from "./pronunciationAssessmentWithRestAPI.js";

if (process.argv.length > 3) {
    settings.filename = process.argv[3];
}

if (process.argv.length > 2) {
    switch (process.argv[2]) {
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
        
        case "pronunciationAssessmentWithRestAPI":
            pronunciationAssessmentWithRestAPI.main(settings);
            break;

        default:
            console.log("Now recognizing speech from: " + settings.filename);
            pronunciationAssessment.main(settings);
            break;
    }
}
else {
    console.log("usage: index.js [pronunciationAssessment|pronunciationAssessmentConfiguredWithJson|pronunciationAssessmentContinue|pronunciationAssessmentFromMicrophone] {filename}");
}
