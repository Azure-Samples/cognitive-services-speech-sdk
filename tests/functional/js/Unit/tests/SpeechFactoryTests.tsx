//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import { WaveFileAudioInputStream } from "./WaveFileAudioInputStream";

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

    expect(() => s.createSpeechRecognizerWithFileInput(null)).toThrowError();

    s.close();
});

test.skip("testCreateSpeechRecognizerString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithFileInput("illegal-" + Settings.WaveFile)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// Test
test("testCreateSpeechRecognizerStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithFileInputAndLanguage(null, null)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithFileInputAndLanguage(null, "en-EN")).toThrowError();

    s.close();
});

test.skip("testCreateSpeechRecognizerStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithFileInputAndLanguage(Settings.WaveFile, "illegal-language")).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInputAndLanguage(Settings.WaveFile, "en-EN");

    expect(r).not.toBeUndefined();

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStream1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithStream(null)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStreamSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStreamString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithStreamAndLanguage(null, null)).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStreamString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => s.createSpeechRecognizerWithStreamAndLanguage(null, "en-US")).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStreamString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    expect(() => s.createSpeechRecognizerWithStreamAndLanguage(ais, null)).toThrowError();

    s.close();
});

test.skip("testCreateSpeechRecognizerAudioInputStreamString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    expect(() => s.createSpeechRecognizerWithStreamAndLanguage(ais, "illegal-language")).toThrowError();

    s.close();
});

test("testCreateSpeechRecognizerAudioInputStreamStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(ais, "en-US");
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    ais.close();
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

test.skip("testCreateIntentRecognizerString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput("illegal-" + Settings.WaveFile);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput(Settings.WaveFile);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateIntentRecognizerAudioInputStream1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStream(null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerAudioInputStreamSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStream(ais);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    s.close();
});

test("testCreateIntentRecognizerAudioInputStreamString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerAudioInputStreamString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(null, "en-US");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateIntentRecognizerAudioInputStreamString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(ais, "illegal-language");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateIntentRecognizerAudioInputStreamStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(ais, "en-US");

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

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
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput("illegal-" + Settings.WaveFile, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "illegal", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", null);
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

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
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

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(null, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice("illegal-" + Settings.WaveFile, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "illegal", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString7", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString8", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-US"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString9", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-US"];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, "illegal");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, "en-US");
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "illegal", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(null, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, null, null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString3", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "illegal", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString4", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", null, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString5", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString6", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString7", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString8", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];

        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, "illegal");
        fail("not expected");
    } catch (ex) {
        // expected
    }

    s.close();
});

test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringStringSuccess", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, "en-US");
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
