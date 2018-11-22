// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
module.exports = {
    transform: {
        "^.+\\.tsx?$": "ts-jest",
    },
    testRegex: "tests/.*\\.tsx$",
    testPathIgnorePatterns: ["/lib/", "/node_modules/"],
    moduleFileExtensions: ["ts", "tsx", "js", "jsx", "json", "node"],
    collectCoverage: true,
    "reporters": [ "default", "jest-junit" ],
    setupTestFrameworkScriptFile:"../../../../../build/source/bindings/js/tests/Unit/TestConfiguration.ts"
};
