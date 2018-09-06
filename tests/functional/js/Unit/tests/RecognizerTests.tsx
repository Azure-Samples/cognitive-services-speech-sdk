//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";

beforeEach(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

test("testRecognizer1", () => {
    const s = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const r = s.createIntentRecognizerWithFileInput(Settings.WaveFile);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test("testRecognizer2", () => {
    const s = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const r = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});
/*
// TODO does not work with microphone
test.skip("testRecognizer3", () => {
    const s = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    let targets: List<string> = new List<string>();
    targets.Add("en-US");

    const r = s.createTranslationRecognizer("en-US", targets.ToArray());
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});
*/
