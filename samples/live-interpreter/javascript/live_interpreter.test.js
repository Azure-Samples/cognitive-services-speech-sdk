// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

const assert = require("node:assert/strict");
const { spawnSync } = require("node:child_process");
const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");
const { test } = require("node:test");
const sdk = require("microsoft-cognitiveservices-speech-sdk");
const sample = require("./live_interpreter");

test("missing configuration exits with an actionable error", () => {
    const env = { ...process.env };
    delete env.SPEECH_KEY;
    const result = spawnSync(
        process.execPath,
        [path.join(__dirname, "live_interpreter.js")],
        { env, encoding: "utf8", timeout: 15000 }
    );
    assert.notEqual(result.status, 0);
    assert.match(result.stderr, /SPEECH_KEY/);
});

test("configuration preserves LLM and existing endpoint parameters", () => {
    const endpoint = "wss://example.cognitiveservices.azure.com/" +
        "stt/speech/universal/v2?existing=value&setfeature=llm";
    const config = sample.createTranslationConfig("test-key", endpoint);
    assert.equal(config.voiceName, "personal-voice");
    assert.deepEqual(config.targetLanguages, ["fr"]);
    assert.equal(
        config.speechSynthesisOutputFormat,
        sdk.SpeechSynthesisOutputFormat.Riff16Khz16BitMonoPcm
    );
    assert.equal(
        config.getProperty(sdk.PropertyId.SpeechServiceConnection_Endpoint),
        endpoint
    );
    config.close();
});

test("synthesized audio does not overwrite existing files", () => {
    const directory = fs.mkdtempSync(path.join(os.tmpdir(), "interpreter-"));
    try {
        const file = path.join(directory, "translation-1.wav");
        sample.writeAudio(file, Buffer.from("first"));
        assert.equal(fs.readFileSync(file, "utf8"), "first");
        assert.throws(
            () => sample.writeAudio(file, Buffer.from("second")),
            { code: "EEXIST" }
        );
        assert.equal(fs.readFileSync(file, "utf8"), "first");
    } finally {
        fs.rmSync(directory, { recursive: true });
    }
});
