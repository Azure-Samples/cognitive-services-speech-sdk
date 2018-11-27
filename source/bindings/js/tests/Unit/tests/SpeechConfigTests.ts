// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { isString } from "util";
import * as sdk from "../../../microsoft.cognitiveservices.speech.sdk";
import { createNoDashGuid } from "../../../src/common/Guid";
import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

// Test cases are run linerally, the only other mechanism to demark them in the output is to put a console line in each case and
// report the name.
// tslint:disable-next-line:no-console
beforeEach(() => console.info("---------------------------------------Starting test case-----------------------------------"));

test("Null Param Check, both.", () => {
    expect(() => sdk.SpeechConfig.fromSubscription(null, null)).toThrowError();
});

test("Null Param Check, Region", () => {
    expect(() => sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, null)).toThrowError();
});

test("Null Param Check, Key", () => {
    expect(() => sdk.SpeechConfig.fromSubscription(null, Settings.SpeechRegion)).toThrowError();
});

test("Valid Params", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.close();
});

test("From Endpoint, but null", () => {
    expect(() => sdk.SpeechConfig.fromEndpoint(null, null)).toThrowError();
});

test("From Endpoint, endpoihnt numm null", () => {
    expect(() => sdk.SpeechConfig.fromEndpoint(null, Settings.SpeechSubscriptionKey)).toThrowError();
});

test("From Endpoint, key null", () => {
    expect(() => sdk.SpeechConfig.fromEndpoint(new URL("http://www.example.com"), null)).toThrowError();
});

test.skip("From endpoint, invalid key format.", () => {
    expect(() => sdk.SpeechConfig.fromEndpoint(new URL("http://www.example.com"), "illegal-subscription")).toThrowError();
});

// TODO use an endpoint that we control so the subscription key is not leaked!
test("From endpoing, valid Params", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromEndpoint(new URL("http://www.example.com"), "Settings.SpeechSubscriptionKey");
    expect(s).not.toBeUndefined();
    s.close();
});

test("TypedParametersAccessableViaPropBag", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    TestParam(() => s.authorizationToken, (val: string) => (s.authorizationToken = val), sdk.PropertyId.SpeechServiceAuthorization_Token, s);
    TestParam(() => s.endpointId, (val: string) => (s.endpointId = val), sdk.PropertyId.SpeechServiceConnection_EndpointId, s);
    TestParam(() => s.speechRecognitionLanguage, (val: string) => (s.speechRecognitionLanguage = val), sdk.PropertyId.SpeechServiceConnection_RecoLanguage, s);
});

const TestParam = (getAccess: () => string, setAccess: (val: string) => void, propEnum: sdk.PropertyId, config: sdk.SpeechConfig): void => {
    const testString: string = createNoDashGuid();

    setAccess(testString);
    expect(config.getProperty(sdk.PropertyId[propEnum])).toEqual(testString);
    expect(getAccess()).toEqual(testString);

    const testString2: string = createNoDashGuid();
    config.setProperty(sdk.PropertyId[propEnum], testString2);
    expect(config.getProperty(sdk.PropertyId[propEnum])).toEqual(testString2);
    expect(getAccess()).toEqual(testString2);
};

test("Unset param return default", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const name: string = createNoDashGuid();
    const value: string = createNoDashGuid();

    expect(s.getProperty(name, value)).toEqual(value);
    expect(s.getProperty(name)).toBeUndefined();

    s.close();
});

test("Create Recognizer", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Proeprties are passed to recognizer", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = createNoDashGuid();
    s.authorizationToken = createNoDashGuid();
    s.endpointId = createNoDashGuid();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    expect(r.authorizationToken).toEqual(s.authorizationToken);
    expect(r.endpointId).toEqual(s.endpointId);
    expect(r.speechRecognitionLanguage).toEqual(s.speechRecognitionLanguage);

    r.close();
    s.close();
});

test("Create SR from AudioConfig", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Null Language Throws", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = null;

    expect(() => new sdk.SpeechRecognizer(s)).toThrowError();

    s.close();
});

test("Create recognizer with language and audioConfig", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-EN";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);

    expect(r).not.toBeUndefined();

    s.close();
});

test("Create Intent Recognizer", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s);

    s.close();
});

test("testCreateIntentRecognizerLanguage1", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = null;

    expect(() => new sdk.IntentRecognizer(s)).toThrow();

    s.close();
});

test("Intent Recognizer Success", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Intent Recognizer with Wave File.", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Intent Recognizer null language Throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recognizer No Target Languages Throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-EN";

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recognizer No Source Language Throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.addTargetLanguage("en-EN");

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recog success", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-EN";
    s.addTargetLanguage("en-US");

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Translation Recognizer Null target languages throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], null);
    s.speechRecognitionLanguage = "illegal";

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Test Translation Recognizer emty target list throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.speechRecognitionLanguage = "en-EN";
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "");

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();
    s.close();
});

test("Translation Null voice value throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "illegal");
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], null);
    s.speechRecognitionLanguage = "en-EN";

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translition Recognizer success.", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "en-US");
    s.speechRecognitionLanguage = "en-EN";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("Translation Recog Nul via prop set for targets", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], null);

    expect(() => { s.speechRecognitionLanguage = null; }).toThrow();

    s.close();
});

test("Translation Recog Null via propset voice and targets", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], null);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], null);

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recog Null via propset voice and targets with Language", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], null);
    s.speechRecognitionLanguage = "en-US";
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], null);

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recog with empty targets via prop", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "");
    s.speechRecognitionLanguage = "en-US";
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], null);

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recog, Null voice via prop throws", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "en-US");
    s.speechRecognitionLanguage = "en-US";
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], null);

    expect(() => new sdk.TranslationRecognizer(s)).toThrow();

    s.close();
});

test("Translation Recog success", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages], "en-US");
    s.speechRecognitionLanguage = "en-US";
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], "en-US");

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testClose", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    s.close();
});
