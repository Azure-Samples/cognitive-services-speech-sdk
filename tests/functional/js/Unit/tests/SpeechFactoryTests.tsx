//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

import { isString } from "util";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

test("testFromSubscription1", () => {
    expect(() => sdk.SpeechFactory.fromSubscription(null, null)).toThrowError();
});

test("testFromSubscription2()", () => {
    expect(() => sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, null)).toThrowError();
});

test("testFromSubscription3", () => {
    expect(() => sdk.SpeechFactory.fromSubscription(null, Settings.SpeechRegion)).toThrowError();
});

test.skip("testFromSubscription4", () => {
    expect(() => sdk.SpeechFactory.fromSubscription("illegal", "illegal")).toThrowError();
});

test("testFromSubscriptionSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();

    s.close();
});

test("testGetSubscriptionKey", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();
    expect(Settings.SpeechSubscriptionKey).toEqual(s.subscriptionKey);

    s.close();
});

test("testGetRegion", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();
    expect(Settings.SpeechRegion).toEqual(s.region);

    s.close();
});

test("testFromEndpoint1", () => {
    expect(() => sdk.SpeechFactory.fromEndpoint(null, null)).toThrowError();
});

test("testFromEndpoint2", () => {
    expect(() => sdk.SpeechFactory.fromEndpoint(null, Settings.SpeechRegion)).toThrowError();
});

test("testFromEndpoint3", () => {
    expect(() => sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), null)).toThrowError();
});

test.skip("testFromEndpoint4", () => {
    expect(() => sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), "illegal-subscription")).toThrowError();
});

test("testFromEndpointSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), Settings.SpeechSubscriptionKey);

    expect(s).not.toBeUndefined();

    s.close();
});

test("testGetParameters1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s.parameters).not.toBeNull();
    expect(s.parameters).not.toBeUndefined();

    //   expect(s.region).toEqual(s.parameters.get(sdk.FactoryParameterNames.Region));
    //   expect(s.subscriptionKey == s.parameters.get(FactoryParameterNames.SubscriptionKey));
    expect(s.region).toEqual(s.parameters.get("SPEECH-Region", null));
    expect(s.subscriptionKey).toEqual(s.parameters.get("SPEECH-SubscriptionKey", null));

    s.close();
});

test("testGetParametersString", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const name: string = "stringName";
    const value: string = "stringValue";
    const p: sdk.ISpeechProperties = s.parameters;
    expect(p).not.toBeUndefined();

    p.set(name, value);

    expect(isString(p.get(name, null)));

    expect(value).toEqual(p.get(name, null));
    expect(value).toEqual(p.get(name, "some-other-default-" + value));

    s.close();
});

test("testCreateSpeechRecognizer", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizer();

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateSpeechRecognizerLanguage1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithLanguage(null)).toThrowError();

    s.close();
});

test.skip("testCreateSpeechRecognizerLanguage2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s.createSpeechRecognizerWithLanguage("illegal-language")).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerLanguageSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithLanguage("en-US");

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateSpeechRecognizerString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerFromConfig(null)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerFromConfig(config);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// Test
test("testCreateSpeechRecognizerStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerFromConfigAndLanguage(null, null)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerFromConfigAndLanguage(null, "en-EN")).toThrowError();

    s.close();
});

test.skip("testCreateSpeechRecognizerStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    expect(() => s.createSpeechRecognizerFromConfigAndLanguage(config, "illegal-language")).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerFromConfigAndLanguage(config, "en-EN");

    expect(r).not.toBeUndefined();

    s.close();
});
test("testCreateIntentRecognizer", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizer();

    s.close();
});

test("testCreateIntentRecognizerLanguage1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage(null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateIntentRecognizerLanguage2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage("illegal-language");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerLanguageSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage("en-US");

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateIntentRecognizerString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput(null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput(config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer(null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("illegal", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerStringArrayListOfString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice(null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("illegal", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerStringArrayListOfStringString7", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, "illegal");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerStringArrayListOfStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, "en-US");
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig("illegal-" + Settings.WaveFile, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, "illegal", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, "en-US", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, "en-US", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString7", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, "en-US", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfig(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(null, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "illegal", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString7", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const targets: string[] = ["illegal"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString8", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const targets: string[] = ["en-US"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString9", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    try {
        const targets: string[] = ["en-US"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", targets, "illegal");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerFromConfigAndVoice(config, "en-US", targets, "en-US");
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testClose", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    s.close();
});

test("testGetFactoryImpl", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();

});
