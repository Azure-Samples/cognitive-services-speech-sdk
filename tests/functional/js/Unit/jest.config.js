module.exports = {
    transform: {
        "^.+\\.tsx?$": "ts-jest",
    },
    testRegex: "tests/.*\\.tsx$",
    testPathIgnorePatterns: ["/lib/", "/node_modules/"],
    moduleFileExtensions: ["ts", "tsx", "js", "jsx", "json", "node"],
    collectCoverage: true,
    "reporters": [ "default", "jest-junit" ],
    setupTestFrameworkScriptFile:"../../../../build/tests/functional/js/Unit/TestConfiguration.ts"
};
