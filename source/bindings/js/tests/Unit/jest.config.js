// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
module.exports = {
    transform: {
        "^.+\\.ts$": "ts-jest",
    },
    testRegex: "tests/.*Tests\\.ts$",
    testPathIgnorePatterns: ["/lib/", "/node_modules/"],
    moduleFileExtensions: ["ts", "js", "jsx", "json", "node"],
    collectCoverage: true,
    "reporters": [ "default", "jest-junit" ],
    setupTestFrameworkScriptFile:"../../../../../build/source/bindings/js/tests/Unit/TestConfiguration.ts"
};
