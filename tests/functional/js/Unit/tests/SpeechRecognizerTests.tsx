//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import * as sdk from "../../../../../source/bindings/js/distrib/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import { WaveFileAudioInputStream } from "./WaveFileAudioInputStream";
import { promises } from "fs";
// import { SpeechRecognitionResult, CreateRecognizerWithFileAudioSource } from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { setTimeout } from "timers";
import { settings } from "cluster";
import { WaitForCondition } from "./Utilities";
import { doesNotReject } from "assert";

const FIRST_EVENT_ID: number = 1;
// private AtomicInteger eventIdentifier = new AtomicInteger(FIRST_EVENT_ID);

let eventIdentifier: number;

beforeAll(() => {
    // override inputs, if necessary
    Settings.LoadSettings();
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Ignore
// @Test
test.skip("testDispose", ()=> {
    // todo: make dispose method public
    fail("dispose not yet public");
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testSpeechRecognizer1", ()=> {

    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
    expect(r).not.toBeUndefined();
// ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});
/*
// @Test
test("testSpeechRecognizer2", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    expect(r).not.toBeUndefined();
// ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.recognizeAsync(
        (result) => {
            expect(result).not.toBeUndefined();
            expect("What's the weather like?" === result.text);
            done();
            r.close();
            s.close();
        },
        (err) => {
            fail(err);
            done();

            r.close();
            s.close();
        });
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testGetDeploymentId", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    expect(r).not.toBeUndefined();

    expect(r.deploymentId).not.toBeUndefined();

    r.close();
    s.close();
});

// @Test
test("testSetDeploymentId", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    expect(r).not.toBeUndefined();

    expect(r.deploymentId).not.toBeUndefined();

    const newDeploymentId: string= "new-" + r.deploymentId;
    r.deploymentId = newDeploymentId;

    expect(newDeploymentId === r.deploymentId);

    r.close();
    s.close();
});
*/
// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testGetLanguage1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    expect(r).not.toBeUndefined();

    expect(r.language).not.toBeNull();

    r.close();
    s.close();
});

// @Test
test("testGetLanguage2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const language: string = "de-DE";
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(ais, language);
    expect(r).not.toBeUndefined();

    expect(r.language).not.toBeNull();
    expect(language === r.language);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testGetOutputFormatDefault", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
    expect(r).not.toBeUndefined();

    expect(r.outputFormat === sdk.OutputFormat.Simple);

    r.close();
    s.close();
});

// @Test
test("testGetOutputFormatDetailed", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const language:string = "de-DE";
    const r: sdk.SpeechRecognizer =
     s.createSpeechRecognizerWithFileInputAndLanguageAndOutput(Settings.WaveFile, language, sdk.OutputFormat.Detailed);
    expect(r).not.toBeUndefined();

    expect(r.outputFormat === sdk.OutputFormat.Detailed);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testGetParameters", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
    expect(r).not.toBeUndefined();

    expect(r.parameters).not.toBeUndefined();
    // expect(r.language ==  r.parameters.get(RecognizerParameterNames.SpeechRecognitionLanguage));
    // expect(r.deploymentId == r.parameters.get(RecognizerParameterNames.SpeechModelId)); // TODO: is this really the correct mapping?
    expect(r.language ===  r.parameters.get("SPEECH-Language", null));
    expect(r.deploymentId === r.parameters.get("SPEECH=ModelId", null)); // todo: is this really the correct mapping?

    r.close();
    s.close();
});
// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testRecognizeAsync1", done => {

    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.recognizeAsync(
        (p2) => {
            const res: sdk.SpeechRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized === res.reason);
            expect("What's the weather like?" === res.text);

            r.close();
            s.close();
            done();

        },
        (error) => {
            console.error("Error: " + error);
            expect(false).toEqual(true);

            r.close();
            s.close();
            done();
        });
});

// @Ignore("TODO why does not get whats the weather like")
// @Test
test("testRecognizeAsync2", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    const eventsMap: { [id: string]: number; } = {};
    eventIdentifier = 1;

    r.FinalResultReceived = (o, e) => {
        eventsMap["FinalResultReceived"] = eventIdentifier++;
    };

    r.IntermediateResultReceived = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap["IntermediateResultReceived-" + Date.now().toPrecision(4)] = now;
        eventsMap["IntermediateResultReceived"] = now;
    };

    r.RecognitionErrorRaised = (o, e) => {
        eventsMap["RecognitionErrorRaised"] = eventIdentifier++;
    };

    // todo eventType should be renamed and be a function getEventType()
    r.RecognitionEvent = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap[e.eventType.toString()] = now;
    };

    r.SessionEvent = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap["session" + e.eventType.toString()] = now;
        eventsMap["session" + e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
    };

    // note: TODO session stopped event not necessarily raised before async operation returns!
    //       this makes this test flaky

    r.recognizeAsync(
        (res: sdk.SpeechRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect("What's the weather like?").toEqual(res.text);
            expect(res.reason).toEqual(sdk.RecognitionStatus.Recognized);

            // session events are first and last event
            const LAST_RECORDED_EVENT_ID: number = eventIdentifier;

            expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);

            expect("session" + sdk.SessionEventType.SessionStartedEvent in eventsMap).toEqual(true);
            expect(eventsMap["session" + sdk.SessionEventType.SessionStartedEvent.toString()]).toEqual(FIRST_EVENT_ID);

            if ("session" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap["session" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }
            // end events come after start events.
            if ("session" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(eventsMap["session" + sdk.SessionEventType.SessionStartedEvent.toString()])
                    .toBeLessThan(eventsMap["session" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }

            expect(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()])
                .toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);
            expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

            // make sure, first end of speech, then final result
            expect((LAST_RECORDED_EVENT_ID - 2)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

            expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap["FinalResultReceived"]);

            // recognition events come after session start but before session end events
            expect(eventsMap["session" + sdk.SessionEventType.SessionStartedEvent.toString()])
                .toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

            if ("session" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()])
                    .toBeLessThan(eventsMap["session" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }

            // there is no partial result reported after the final result
            // (and check that we have intermediate and final results recorded)
            if ("IntermediateResultReceived" in eventsMap) {
                expect(eventsMap["IntermediateResultReceived"])
                    .toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);
            }

            // speech should stop before getting the final result.
            expect(eventsMap["FinalResultReceived"]).toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

            expect(eventsMap["IntermediateResultReceived"]).toBeLessThan(eventsMap["FinalResultReceived"]);

            // make sure events we don't expect, don't get raised
            expect("RecognitionErrorRaised" in eventsMap).toBeFalsy();

            r.close();
            s.close();
            done();
        }, (error: string) => {

            r.close();
            s.close();

            fail(error);
            done();
        });

}, 15000);

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testStartContinuousRecognitionAsync", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);

    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.startContinuousRecognitionAsync(
        () => {
            done();
        },
        (err) => {
            expect(false).toEqual(true);
            done();
        });

    r.close();
    s.close();
});

// @Test
test("testStopContinuousRecognitionAsync", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.startContinuousRecognitionAsync(
        () => {
            r.stopContinuousRecognitionAsync(
                () => {
                    done();
                },
                (err) => {
                    expect(false).toEqual(true);
                    done();
                });
        },
        (err) => {
            expect(false).toEqual(true);
            done();
        });

    setTimeout(() => {
        // empty
    }, 1000);

    r.close();
    s.close();
});

// @Test
test("testStartStopContinuousRecognitionAsync", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    const rEvents: string[] = [];

    r.FinalResultReceived = (o, e) => {
        rEvents.push("Result@" + Date.now().toPrecision(4).toString() + ": " + e.result.reason + "/" + e.result.text);
    };

    r.startContinuousRecognitionAsync(
        // wait for max 30 seconds
        () => {
            done();
        },
        (error) => {
            fail(error);
            r.close();
            s.close();
            done();
        });
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Test
test("testGetRecoImpl", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// ---
// -----------------------------------------------------------------------

// @Ignore
// @Test
test.skip("testRecognizer", () => {
    // todo: constructor is protected,
    fail("Not yet implemented");
});

// @Test
test("testClose", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const wavFile: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(wavFile); expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});
