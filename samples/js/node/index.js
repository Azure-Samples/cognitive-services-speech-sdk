// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

(function() {
    "use strict";
    
    var settings = require("./settings");
    var speech = require("./speech");
    var intent = require("./intent");
    var translate = require("./translation");
    var synthesis = require("./synthesis");
    var pronunciationAssessmentContinue = require("./pronunciationAssessmentContinue");
    var pronunciationAssessment = require("./pronunciationAssessment");
    var pronunciationAssessmentConfiguredWithJson = require("./pronunciationAssessmentConfiguredWithJson");
    var pronunciationAssessmentFromMicrophone = require("./pronunciationAssessmentFromMicrophone");
    
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
}());
    