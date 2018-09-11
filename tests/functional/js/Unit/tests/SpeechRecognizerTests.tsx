//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { promises } from "fs";
import { setTimeout } from "timers";
import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

const FIRST_EVENT_ID: number = 1;
const IntermediateResultReceived: string = "IntermediateResultReceived";
const FinalResultReceived: string = "FinalResultReceived";
const Session: string = "Session";
const RecognitionErrorRaised: string = "RecognitionErrorRaised";

let eventIdentifier: number;

beforeAll(() => {
    // override inputs, if necessary
    Settings.LoadSettings();
});

test.skip("testDispose", () => {
    // todo: make dispose method public
    fail("dispose not yet public");
});

test("testSpeechRecognizer1", () => {

    const speechConfig: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(speechConfig).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(speechConfig, config);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
});
/*

test("testSpeechRecognizer2", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
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
        (err:string) => {
            fail(err:string);
            done();

            r.close();
            s.close();
        });
});

test("testGetDeploymentId", ()=> {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    expect(r).not.toBeUndefined();

    expect(r.deploymentId).not.toBeUndefined();

    r.close();
    s.close();
});

test("testSetDeploymentId", ()=> {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
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

test("testGetLanguage1", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.language).not.toBeNull();

    r.close();
    s.close();
});

test("testGetLanguage2", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = "de-DE";
    s.language = language;

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.language).not.toBeNull();
    expect(language === r.language);

    r.close();
    s.close();
});

test("testGetOutputFormatDefault", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.outputFormat === sdk.OutputFormat.Simple);

    r.close();
    s.close();
});

test("testGetOutputFormatDetailed", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = "de-DE";
    s.language = language;
    s.setProperty(sdk.RecognizerParameterNames.OutputFormat, "DETAILED");

    const r: sdk.SpeechRecognizer =
        new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.outputFormat === sdk.OutputFormat.Detailed);

    r.close();
    s.close();
});

test("testGetParameters", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.parameters).not.toBeUndefined();
    // expect(r.language ==  r.parameters.get(RecognizerParameterNames.SpeechRecognitionLanguage));
    // expect(r.deploymentId == r.parameters.get(RecognizerParameterNames.SpeechMspeechConfigImpl// TODO: is this really the correct mapping?
    expect(r.language).not.toBeUndefined();
    expect(r.deploymentId === r.parameters.get("SPEECH=ModelId", null)); // todo: is this really the correct mapping?

    r.close();
    s.close();
});

test("testRecognizeAsync1", (done: jest.DoneCallback) => {

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.language = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized === res.reason);
            expect("What's the weather like?" === res.text);

            r.close();
            s.close();
            done();

        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("testRecognizeAsync2", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.language = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    const eventsMap: { [id: string]: number; } = {};
    eventIdentifier = 1;

    r.FinalResultReceived = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
        eventsMap[FinalResultReceived] = eventIdentifier++;
    };

    r.IntermediateResultReceived = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[IntermediateResultReceived + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[IntermediateResultReceived] = now;
    };

    r.RecognitionErrorRaised = (o: sdk.Recognizer, e: sdk.RecognitionErrorEventArgs) => {
        eventsMap[RecognitionErrorRaised] = eventIdentifier++;
    };

    // todo eventType should be renamed and be a function getEventType()
    r.RecognitionEvent = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[e.eventType.toString()] = now;
    };

    r.SessionEvent = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + e.eventType.toString()] = now;
        eventsMap[Session + e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
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

            expect(Session + sdk.SessionEventType.SessionStartedEvent in eventsMap).toEqual(true);
            expect(eventsMap[Session + sdk.SessionEventType.SessionStartedEvent.toString()]).toEqual(FIRST_EVENT_ID);

            if (Session + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap[Session + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }
            // end events come after start events.
            if (Session + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(eventsMap[Session + sdk.SessionEventType.SessionStartedEvent.toString()])
                    .toBeLessThan(eventsMap[Session + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }

            expect(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()])
                .toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);
            expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

            // make sure, first end of speech, then final result
            expect((LAST_RECORDED_EVENT_ID - 2)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

            expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[FinalResultReceived]);

            // recognition events come after session start but before session end events
            expect(eventsMap[Session + sdk.SessionEventType.SessionStartedEvent.toString()])
                .toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

            if (Session + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()])
                    .toBeLessThan(eventsMap[Session + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }

            // there is no partial result reported after the final result
            // (and check that we have intermediate and final results recorded)
            if (IntermediateResultReceived in eventsMap) {
                expect(eventsMap[IntermediateResultReceived])
                    .toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);
            }

            // speech should stop before getting the final result.
            expect(eventsMap[FinalResultReceived]).toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

            expect(eventsMap[IntermediateResultReceived]).toBeLessThan(eventsMap[FinalResultReceived]);

            // make sure events we don't expect, don't get raised
            expect(RecognitionErrorRaised in eventsMap).toBeFalsy();

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

test("testStartContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);

    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.startContinuousRecognitionAsync(
        () => {
            done();
        },
        (err: string) => {
            expect(false).toEqual(true);
            done();
        });

    r.close();
    s.close();
});

test("testStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.startContinuousRecognitionAsync(
        () => {
            r.stopContinuousRecognitionAsync(
                () => {
                    done();
                },
                (err: string) => {
                    expect(false).toEqual(true);
                    done();
                });
        },
        (err: string) => {
            expect(false).toEqual(true);
            done();
        });

    r.close();
    s.close();
});

test("testStartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    const rEvents: string[] = [];

    r.FinalResultReceived = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
        rEvents.push("Result@" + Date.now().toPrecision(4).toString() + ": " + e.result.reason + "/" + e.result.text);
    };

    r.startContinuousRecognitionAsync(
        // wait for max 30 seconds
        () => {
            done();
        },
        (error: string) => {
            fail(error);
            r.close();
            s.close();
            done();
        });
});

test("testGetRecoImpl", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    // ??     assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

test.skip("testRecognizer", () => {
    // todo: constructor is protected,
    fail("Not yet implemented");
});

test("testClose", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});
