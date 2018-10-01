//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener } from "../../../../../source/bindings/js/src/common.browser/Exports";
import { Events, EventType } from "../../../../../source/bindings/js/src/common/Exports";

import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

import { setTimeout } from "timers";
import WaitForCondition from "./Utilities";

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let eventIdentifier: number;
let errorText: string;

beforeAll(() => {
    // override inputs, if necessary
    Settings.LoadSettings();
    Events.Instance.AttachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, the only other mechanism to demark them in the output is to put a console line in each case and
// report the name.
beforeEach(() => {
    errorText = undefined;
    // tslint:disable-next-line:no-console
    console.info("---------------------------------------Starting test case-----------------------------------");
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

    expect(r instanceof sdk.Recognizer);

    r.close();
});

test("testGetLanguage1", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.speechRecognitionLanguage).not.toBeNull();

    r.close();
    s.close();
});

test("testGetLanguage2", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = "de-DE";
    s.speechRecognitionLanguage = language;

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.speechRecognitionLanguage).not.toBeNull();
    expect(language === r.speechRecognitionLanguage);

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

test("testGetOutputFormatDetailed", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = Settings.WaveFileLanguage;
    s.speechRecognitionLanguage = language;
    s.outputFormat = sdk.OutputFormat.Detailed;

    const r: sdk.SpeechRecognizer =
        new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r.outputFormat === sdk.OutputFormat.Detailed);

    r.recognizeOnceAsync((result: sdk.SpeechRecognitionResult) => {
        expect(result.text).toEqual(Settings.WaveFileText);

        r.close();
        s.close();
        done();
    }, (error: string) => {
        setTimeout(done, 1);
        fail(error);
    });
});

test("testGetParameters", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.properties).not.toBeUndefined();
    // expect(r.language ==  r.properties.getProperty(RecognizerParameterNames.SpeechRecognitionLanguage));
    // expect(r.deploymentId == r.properties.getProperty(RecognizerParameterNames.SpeechMspeechConfigImpl// TODO: is this really the correct mapping?
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.endpointId === r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_EndpointId, null)); // todo: is this really the correct mapping?

    r.close();
    s.close();
});

test("testRecognizeOnceAsync1", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect("What's the weather like?").toEqual(res.text);
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("testRecognizeOnceAsync2", (done: jest.DoneCallback) => {
    const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
    const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
    const SessionStartedEvent = "SessionStartedEvent";
    const SessionStoppedEvent = "SessionStoppedEvent";
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const eventsMap: { [id: string]: number; } = {};
    eventIdentifier = 1;

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        eventsMap[Recognized] = eventIdentifier++;
    };

    r.recognizing = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Recognizing + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[Recognizing] = now;
    };

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        eventsMap[Canceled] = eventIdentifier++;
    };

    // todo eventType should be renamed and be a function getEventType()
    r.speechStartDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        // tslint:disable-next-line:no-string-literal
        eventsMap[SpeechStartDetectedEvent] = now;
    };
    r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[SpeechEndDetectedEvent] = now;
    };

    r.sessionStarted = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + SessionStartedEvent] = now;
        eventsMap[Session + SessionStartedEvent + "-" + Date.now().toPrecision(4)] = now;
    };
    r.sessionStopped = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + SessionStoppedEvent] = now;
        eventsMap[Session + SessionStoppedEvent + "-" + Date.now().toPrecision(4)] = now;
    };

    // note: TODO session stopped event not necessarily raised before async operation returns!
    //       this makes this test flaky

    r.recognizeOnceAsync(
        (res: sdk.SpeechRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect(res.text).toEqual("What's the weather like?");
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);

            // session events are first and last event
            const LAST_RECORDED_EVENT_ID: number = eventIdentifier;

            expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);

            expect(Session + SessionStartedEvent in eventsMap).toEqual(true);
            expect(eventsMap[Session + SessionStartedEvent]).toEqual(FIRST_EVENT_ID);

            if (Session + SessionStoppedEvent in eventsMap) {
                expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap[Session + SessionStoppedEvent]);
            }
            // end events come after start events.
            if (Session + SessionStoppedEvent in eventsMap) {
                expect(eventsMap[Session + SessionStartedEvent])
                    .toBeLessThan(eventsMap[Session + SessionStoppedEvent]);
            }

            expect(eventsMap[SpeechStartDetectedEvent])
                .toBeLessThan(eventsMap[SpeechEndDetectedEvent]);
            expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[SpeechStartDetectedEvent]);

            // make sure, first end of speech, then final result
            expect((LAST_RECORDED_EVENT_ID - 2)).toEqual(eventsMap[SpeechEndDetectedEvent]);

            expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[Recognized]);

            // recognition events come after session start but before session end events
            expect(eventsMap[Session + SessionStartedEvent])
                .toBeLessThan(eventsMap[SpeechStartDetectedEvent]);

            if (Session + SessionStoppedEvent in eventsMap) {
                expect(eventsMap[SpeechEndDetectedEvent])
                    .toBeLessThan(eventsMap[Session + SessionStoppedEvent]);
            }

            // there is no partial result reported after the final result
            // (and check that we have intermediate and final results recorded)
            if (Recognizing in eventsMap) {
                expect(eventsMap[Recognizing])
                    .toBeGreaterThan(eventsMap[SpeechStartDetectedEvent]);
            }

            // speech should stop before getting the final result.
            expect(eventsMap[Recognized]).toBeGreaterThan(eventsMap[SpeechEndDetectedEvent]);

            expect(eventsMap[Recognizing]).toBeLessThan(eventsMap[Recognized]);

            // make sure events we don't expect, don't get raised
            expect(Canceled in eventsMap).toBeFalsy();

            r.close();
            s.close();
            done();
        }, (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });

});

test("testStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    let eventDone: boolean = false;
    let canceled: boolean = false;

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        try {
            eventDone = true;
            expect(e.result.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(e.result.text).toEqual("What's the weather like?");
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
        try {
            canceled = true;
            expect(e.errorDetails).toBeUndefined();
            expect(e.reason).toEqual(sdk.CancellationReason.EndOfStream);
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.startContinuousRecognitionAsync(
        () => WaitForCondition(() => (eventDone && canceled), () => {
            r.stopContinuousRecognitionAsync(
                () => {
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    expect(errorText).toBeUndefined();
                },
                (err: string) => {
                    setTimeout(done, 1);
                    fail(err);
                });
        }),
        (err: string) => {
            setTimeout(done, 1);
            fail(err);
        });
});

test("testStartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);

    const rEvents: string[] = [];

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        rEvents.push("Result@" + Date.now().toPrecision(4).toString() + ": " + e.result.reason + "/" + e.result.text);
    };

    r.startContinuousRecognitionAsync(
        // wait for max 30 seconds
        () => {
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("testGetRecoImpl", () => {
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

test("Config is copied on construction", () => {

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const ranVal: string = Math.random().toString();

    s.setProperty("RandomProperty", ranVal);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], "en-US-Zira");

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice])).toEqual("en-US-Zira");

    // Change them.
    s.speechRecognitionLanguage = "de-DE";
    s.setProperty("RandomProperty", Math.random.toString());
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], "de-DE-Hedda");

    // Validate no change.
    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice])).toEqual("en-US-Zira");

});

test("PushStream4KNoDelay", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
    let i: number;

    const sendSize: number = 4096;

    for (i = sendSize - 1; i < f.byteLength; i += sendSize) {
        p.write(f.slice(i - (sendSize - 1), i));
    }

    p.write(f.slice(i - (sendSize - 1), f.byteLength - 1));
    p.close();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.RecognizedSpeech).toEqual(res.reason);
            expect(res.text).toEqual("What's the weather like?");

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("PushStream4KPostRecognizePush", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
    let i: number;

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.RecognizedSpeech).toEqual(res.reason);
            expect(res.text).toEqual("What's the weather like?");

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });

    const sendSize: number = 4096;

    for (i = sendSize - 1; i < f.byteLength; i += sendSize) {
        p.write(f.slice(i - (sendSize - 1), i));
    }

    p.write(f.slice(i - (sendSize - 1), f.byteLength - 1));
    p.close();

});

test("PullStreamFullFill", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const fileBuffer: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);

    let bytesSent: number = 0;
    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                const copyArray: Uint8Array = new Uint8Array(buffer);
                const start: number = bytesSent;
                const end: number = buffer.byteLength > (fileBuffer.byteLength - bytesSent) ? (fileBuffer.byteLength - 1) : (bytesSent + buffer.byteLength - 1);
                copyArray.set(new Uint8Array(fileBuffer.slice(start, end)));
                bytesSent += (end - start) + 1;

                if (bytesSent < buffer.byteLength) {
                    setTimeout(() => p.close(), 1000);
                }

                return (end - start) + 1;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.RecognizedSpeech).toEqual(res.reason);
            expect(res.text).toEqual("What's the weather like?");

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("PullStreamHalfFill", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const fileBuffer: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);

    let bytesSent: number = 0;
    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                const copyArray: Uint8Array = new Uint8Array(buffer);
                const start: number = bytesSent;
                const fillSize: number = Math.round(fileBuffer.byteLength / 2);
                const end: number = buffer.byteLength > (fillSize - bytesSent) ? (fillSize - 1) : (bytesSent + buffer.byteLength - 1);
                copyArray.set(new Uint8Array(fileBuffer.slice(start, end)));
                bytesSent += (end - start) + 1;

                if (bytesSent < buffer.byteLength) {
                    setTimeout(() => p.close(), 1000);
                }

                return (end - start) + 1;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.RecognizedSpeech).toEqual(res.reason);
            expect(res.text).toEqual("What's the weather like?");

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            fail(error);
        });
});

test("InitialSilenceTimeout", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                return buffer.byteLength;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    let oneReport: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            setTimeout(done, 1);
            fail(e.errorDetails);
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        try {
            const res: sdk.SpeechRecognitionResult = e.result;
            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

            if (true === oneReport) {
                expect(errorText).toBeUndefined();
                done();
            }

            oneReport = true;
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

            r.close();
            s.close();
            if (true === oneReport) {
                expect(errorText).toBeUndefined();
                done();
            }

            oneReport = true;
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
}, 7500);

test.skip("InitialBabbleTimeout", (done: jest.DoneCallback) => {

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "es-MX";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect("What's the weather like?").toEqual(res.text);
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);

            r.close();
            s.close();
            done();

        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("emptyFile", (done: jest.DoneCallback) => {
    // Server Responses:
    // turn.start {"context": { "serviceTag": "<tag>"  }}
    // speech.endDetected { }
    // speech.phrase { "RecognitionStatus": "Error", "Offset": 0, "Duration": 0 }

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const blob: Blob[] = [];
    const f: File = new File(blob, "file.wav");

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    let oneCalled: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
        try {
            expect(e.reason).toEqual(sdk.CancellationReason.Error);

            if (true === oneCalled) {
                r.close();
                s.close();
                expect(errorText).toBeUndefined();
                done();
            } else {
                oneCalled = true;
            }
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            expect(p2.reason).toEqual(sdk.ResultReason.Canceled);
            const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(p2);
            expect(cancelDetails.reason).toEqual(sdk.CancellationReason.Error);

            if (true === oneCalled) {
                r.close();
                s.close();
                expect(errorText).toBeUndefined();
                done();
            } else {
                oneCalled = true;
            }

        },
        (error: string) => {
            fail(error);
        });
});

test("PullStreamSendHalfTheFile", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const fileBuffer: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);

    let bytesSent: number = 0;
    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                const copyArray: Uint8Array = new Uint8Array(buffer);
                const start: number = bytesSent;
                const end: number = buffer.byteLength > (fileBuffer.byteLength - bytesSent) ? (fileBuffer.byteLength - 1) : (bytesSent + buffer.byteLength - 1);
                copyArray.set(new Uint8Array(fileBuffer.slice(start, end)));
                bytesSent += (end - start) + 1;

                if (bytesSent > (fileBuffer.byteLength / 2)) {
                    p.close();
                }

                return (end - start) + 1;
            },
        });
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.RecognizedSpeech).toEqual(res.reason);
            expect(res.text).toEqual("What's the weather?");

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("burst of silence", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const emptyBuffer: Uint8Array = new Uint8Array(1 * 1024);
    p.write(emptyBuffer.buffer);
    p.close();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.NoMatch);
            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
});

test("RecognizeOnceAsync is async", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    let postCall: boolean = false;
    let resultSeen: boolean = false;
    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        WaitForCondition(() => postCall, () => {
            resultSeen = true;
            setTimeout(done, 1);
            expect(e.result.errorDetails).toBeUndefined();
            expect(e.result.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(e.result.text).toEqual(Settings.WaveFileText);
        });
    };

    r.recognizeOnceAsync();

    expect(resultSeen).toEqual(false);
    postCall = true;
});

test("InitialSilenceTimeout Continous", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                return buffer.byteLength;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            setTimeout(done, 1);
            fail(e.errorDetails);
        } catch (error) {
            errorText += error.message + " " + error.stack;
        }
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {

        const res: sdk.SpeechRecognitionResult = e.result;
        expect(res).not.toBeUndefined();
        expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
        expect(res.text).toBeUndefined();

        const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
        expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);
        r.stopContinuousRecognitionAsync();
        expect(errorText).toBeUndefined();
        done();
    };

    /* tslint:disable:no-empty */
    r.startContinuousRecognitionAsync(() => {
    },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(done, 1);
            fail(error);
        });
}, 10000);

test("Audio Config is optional", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

test("Default mic is used when audio config is not specified.", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    expect(r instanceof sdk.Recognizer).toEqual(true);
    // Node.js doesn't have a microphone natively. So we'll take the specific message that indicates that microphone init failed as evidence it was attempted.
    r.recognizeOnceAsync(() => fail("RecognizeOnceAsync returned success when it should have failed"),
        (error: string): void => {
            expect(error).toEqual("Error: Browser does not support Web Audio API (AudioContext is not available).");
        });

    r.startContinuousRecognitionAsync(() => fail("startContinuousRecognitionAsync returned success when it should have failed"),
        (error: string): void => {
            expect(error).toEqual("Error: Browser does not support Web Audio API (AudioContext is not available).");
        });
});

test("Using disposed recognizer invokes error callbacks.", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();

    r.recognizeOnceAsync(() => fail("RecognizeOnceAsync on closed recognizer called success callback"),
        (error: string): void => {
            expect(error).toEqual("Error: the object is already disposed");
        });

    r.startContinuousRecognitionAsync(() => fail("startContinuousRecognitionAsync on closed recognizer called success callback"),
        (error: string): void => {
            expect(error).toEqual("Error: the object is already disposed");
        });

    r.stopContinuousRecognitionAsync(() => fail("stopContinuousRecognitionAsync on closed recognizer called success callback"),
        (error: string): void => {
            expect(error).toEqual("Error: the object is already disposed");
        });
});
