// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import * as sdk from "../../../microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener } from "../../../src/common.browser/Exports";
import { ServiceRecognizerBase } from "../../../src/common.speech/Exports";
import { QueryParameterNames } from "../../../src/common.speech/QueryParameterNames";
import { ConnectionStartEvent } from "../../../src/common/Exports";
import { Events, EventType, PlatformEvent } from "../../../src/common/Exports";

import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

import { setTimeout } from "timers";
import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import WaitForCondition from "./Utilities";

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let eventIdentifier: number;
let objsToClose: any[];

beforeAll(() => {
    // override inputs, if necessary
    Settings.LoadSettings();
    Events.Instance.AttachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, the only other mechanism to demark them in the output is to put a console line in each case and
// report the name.
beforeEach(() => {
    objsToClose = [];
    // tslint:disable-next-line:no-console
    console.info("---------------------------------------Starting test case-----------------------------------");
});

afterEach(() => {
    objsToClose.forEach((value: any, index: number, array: any[]) => {
        if (typeof value.close === "function") {
            value.close();
        }
    });
});

const BuildRecognizerFromWaveFile: (speechConfig?: sdk.SpeechConfig) => sdk.SpeechRecognizer = (speechConfig?: sdk.SpeechConfig): sdk.SpeechRecognizer => {

    let s: sdk.SpeechConfig = speechConfig;
    if (s === undefined) {
        s = BuildSpeechConfig();
        // Since we're not going to return it, mark it for closure.
        objsToClose.push(s);
    }

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = Settings.WaveFileLanguage;
    if (s.speechRecognitionLanguage === undefined) {
        s.speechRecognitionLanguage = language;
    }

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    expect(r).not.toBeUndefined();

    return r;
};

const BuildSpeechConfig: () => sdk.SpeechConfig = (): sdk.SpeechConfig => {

    let s: sdk.SpeechConfig;
    if (undefined === Settings.SpeechEndpoint) {
        s = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    } else {
        s = sdk.SpeechConfig.fromEndpoint(new URL(Settings.SpeechEndpoint), Settings.SpeechSubscriptionKey);
    }

    expect(s).not.toBeUndefined();
    return s;
};

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
    objsToClose.push(r);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer);
});

test("testGetLanguage1", () => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.speechRecognitionLanguage).not.toBeNull();
});

test("testGetLanguage2", () => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const language: string = "de-DE";
    s.speechRecognitionLanguage = language;

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    expect(r.speechRecognitionLanguage).not.toBeNull();
    expect(language === r.speechRecognitionLanguage);
});

test("testGetOutputFormatDefault", () => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.outputFormat === sdk.OutputFormat.Simple);
});

test("testGetOutputFormatDetailed", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    s.outputFormat = sdk.OutputFormat.Detailed;

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    expect(r.outputFormat === sdk.OutputFormat.Detailed);

    r.recognizeOnceAsync((result: sdk.SpeechRecognitionResult) => {
        expect(result.text).toEqual(Settings.WaveFileText);

        done();
    }, (error: string) => {
        setTimeout(done, 1);
        fail(error);
    });
});

test("testGetParameters", () => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.properties).not.toBeUndefined();
    // expect(r.language ==  r.properties.getProperty(RecognizerParameterNames.SpeechRecognitionLanguage));
    // expect(r.deploymentId == r.properties.getProperty(RecognizerParameterNames.SpeechMspeechConfigImpl// TODO: is this really the correct mapping?
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.endpointId === r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_EndpointId, null)); // todo: is this really the correct mapping?
});

test("RecognizeOnce", (done: jest.DoneCallback) => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    let telemetryEvents: number = 0;

    ServiceRecognizerBase.TelemetryData = (json: string): void => {
        telemetryEvents++;
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            setTimeout(done, 1);
            const res: sdk.SpeechRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect("What's the weather like?").toEqual(res.text);
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(telemetryEvents).toEqual(1);
        },
        (error: string) => {
            setTimeout(done, 1);
            fail(error);
        });
});

test("Event Tests (RecognizeOnce)", (done: jest.DoneCallback) => {
    const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
    const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
    const SessionStartedEvent = "SessionStartedEvent";
    const SessionStoppedEvent = "SessionStoppedEvent";
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

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
            const LAST_RECORDED_EVENT_ID: number = --eventIdentifier;

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
            expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[SpeechEndDetectedEvent]);

            expect((LAST_RECORDED_EVENT_ID)).toEqual(eventsMap[Recognized]);

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

            done();
        }, (error: string) => {
            setTimeout(done, 1);
            fail(error);
        });

});

test("Event Tests (Continuous)", (done: jest.DoneCallback) => {
    const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
    const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
    const SessionStartedEvent = "SessionStartedEvent";
    const SessionStoppedEvent = "SessionStoppedEvent";
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    let sessionStopped: boolean = false;

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
        sessionStopped = true;
    };

    r.startContinuousRecognitionAsync();

    WaitForCondition(() => sessionStopped, () => {
        // session events are first and last event
        const LAST_RECORDED_EVENT_ID: number = --eventIdentifier;
        expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);

        expect(Session + SessionStartedEvent in eventsMap).toEqual(true);
        expect(eventsMap[Session + SessionStartedEvent]).toEqual(FIRST_EVENT_ID);

        expect(Session + SessionStoppedEvent in eventsMap).toEqual(true);
        expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap[Session + SessionStoppedEvent]);

        // end events come after start events.
        if (Session + SessionStoppedEvent in eventsMap) {
            expect(eventsMap[Session + SessionStartedEvent])
                .toBeLessThan(eventsMap[Session + SessionStoppedEvent]);
        }

        expect(eventsMap[SpeechStartDetectedEvent])
            .toBeLessThan(eventsMap[SpeechEndDetectedEvent]);
        expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[SpeechStartDetectedEvent]);

        // make sure, first end of speech, then final result
        expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[Canceled]);
        expect((LAST_RECORDED_EVENT_ID - 2)).toEqual(eventsMap[SpeechEndDetectedEvent]);
        expect((LAST_RECORDED_EVENT_ID - 3)).toEqual(eventsMap[Recognized]);

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

        // speech should not stop before getting the final result.
        expect(eventsMap[Recognized]).toBeLessThan(eventsMap[SpeechEndDetectedEvent]);

        expect(eventsMap[Recognizing]).toBeLessThan(eventsMap[Recognized]);

        // make sure we got a cancel event.
        expect(Canceled in eventsMap).toEqual(true);

        done();
    });
});

test("testStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    let eventDone: boolean = false;
    let canceled: boolean = false;
    let telemetryEvents: number = 0;

    ServiceRecognizerBase.TelemetryData = (json: string): void => {
        telemetryEvents++;
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        try {
            eventDone = true;
            expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(e.result.text).toEqual("What's the weather like?");
        } catch (error) {
            done.fail(error);
        }
    };

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
        try {
            canceled = true;
            expect(e.errorDetails).toBeUndefined();
            expect(e.reason).toEqual(sdk.CancellationReason.EndOfStream);
        } catch (error) {
            done.fail(error);
        }
    };

    r.startContinuousRecognitionAsync(
        () => WaitForCondition(() => (eventDone && canceled), () => {
            r.stopContinuousRecognitionAsync(
                () => {
                    // Three? One for the phrase that was recognized.
                    // Once for the end of stream.
                    // Once when closed.
                    expect(telemetryEvents).toEqual(3);
                    done();
                },
                (err: string) => {
                    done.fail(err);
                });
        }),
        (err: string) => {
            done.fail(err);
        });
});

test("testStartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

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
            done.fail(error);
        });
});

test("Close with no recognition", () => {
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
});

test("Config is copied on construction", () => {

    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    s.speechRecognitionLanguage = "en-US";

    const ranVal: string = Math.random().toString();

    s.setProperty("RandomProperty", ranVal);
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], "Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)");

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice])).toEqual("Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)");

    // Change them.
    s.speechRecognitionLanguage = "de-DE";
    s.setProperty("RandomProperty", Math.random.toString());
    s.setProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice], "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)");

    // Validate no change.
    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationVoice])).toEqual("Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)");

});

test("PushStream4KNoDelay", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
    let i: number;

    const sendSize: number = 4096;

    for (i = sendSize - 1; i < f.byteLength; i += sendSize) {
        p.write(f.slice(i - (sendSize - 1), i + 1));
    }

    p.write(f.slice(i - (sendSize - 1), f.byteLength));
    p.close();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather like?");

            done();
        },
        (error: string) => {
            setTimeout(done, 1);
            fail(error);
        });
});

test("PushStream4KPostRecognizePush", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
    let i: number;

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather like?");

            done();
        },
        (error: string) => {
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
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

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
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather like?");

            done();
        },
        (error: string) => {
            setTimeout(done, 1);
            fail(error);
        });
});

test("PullStreamHalfFill", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const fileBuffer: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);

    let bytesSent: number = 0;
    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                const copyArray: Uint8Array = new Uint8Array(buffer);
                const start: number = bytesSent;
                const fillSize: number = Math.round(buffer.byteLength / 2);
                const end: number = fillSize > (fileBuffer.byteLength - bytesSent) ? (fileBuffer.byteLength - 1) : (bytesSent + fillSize - 1);
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
    objsToClose.push(r);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather like?");

            done();
        },
        (error: string) => {
            setTimeout(done, 1);

            fail(error);
        });
});

test("InitialSilenceTimeout (pull)", (done: jest.DoneCallback) => {
    let p: sdk.PullAudioInputStream;
    let bytesSent: number = 0;

    // To make sure we don't send a ton of extra data.
    // 5s * 16K * 2 * 1.25;
    // For reference, before the throttling was implemented, we sent 6-10x the required data.
    const expectedBytesSent: number = 5 * 16000 * 2 * 1.25;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                bytesSent += buffer.byteLength;
                return buffer.byteLength;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    testInitialSilienceTimeout(config, done, () => expect(bytesSent).toBeLessThan(expectedBytesSent));
});

test("InitialSilenceTimeout (push)", (done: jest.DoneCallback) => {
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    p.write(bigFileBuffer.buffer);
    p.close();

    testInitialSilienceTimeout(config, done);
});

test("InitialSilenceTimeout (File)", (done: jest.DoneCallback) => {

    const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
    const bigFile: File = ByteBufferAudioFile.Load(bigFileBuffer.buffer);

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(bigFile);

    testInitialSilienceTimeout(config, done);
});

const testInitialSilienceTimeout = (config: sdk.AudioConfig, done: jest.DoneCallback, addedChecks?: () => void): void => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    // To validate the data isn't sent too fast.
    const startTime: number = Date.now();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    let numReports: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        done.fail(e.errorDetails);
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        try {
            const res: sdk.SpeechRecognitionResult = e.result;
            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);
        } catch (error) {
            done.fail(error);
        } finally {
            numReports++;
        }

    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;
            numReports++;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.errorDetails).toBeUndefined();
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);
            expect(Date.now()).toBeGreaterThan(startTime + 2400);

        },
        (error: string) => {
            fail(error);
        });

    WaitForCondition(() => (numReports === 2), () => {
        setTimeout(done, 1);
        if (!!addedChecks) {
            addedChecks();
        }
    });
};

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

    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const blob: Blob[] = [];
    const f: File = new File(blob, "file.wav");

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    let oneCalled: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
        try {
            expect(e.reason).toEqual(sdk.CancellationReason.Error);

            if (true === oneCalled) {
                done();
            } else {
                oneCalled = true;
            }
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            expect(p2.reason).toEqual(sdk.ResultReason.Canceled);
            const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(p2);
            expect(cancelDetails.reason).toEqual(sdk.CancellationReason.Error);

            if (true === oneCalled) {
                done();
            } else {
                oneCalled = true;
            }

        },
        (error: string) => {
            done.fail(error);
        });
});

test("PullStreamSendHalfTheFile", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

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
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather?");

            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("burst of silence", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const emptyBuffer: Uint8Array = new Uint8Array(1 * 1024);
    p.write(emptyBuffer.buffer);
    p.close();

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.NoMatch);
            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("RecognizeOnceAsync is async", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);
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
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

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
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            fail(e.errorDetails);
            done();
        } catch (error) {
            done.fail(error);
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
        done();
    };

    /* tslint:disable:no-empty */
    r.startContinuousRecognitionAsync(() => {
    },
        (error: string) => {
            done.fail(error);
        });
}, 10000);

test("Audio Config is optional", () => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    objsToClose.push(r);
    expect(r instanceof sdk.Recognizer).toEqual(true);
});

test("Default mic is used when audio config is not specified.", () => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
    objsToClose.push(r);

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
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

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

test.skip("Endpoint URL Test", (done: jest.DoneCallback) => {
    let uri: string;

    Events.Instance.AttachListener({
        OnEvent: (event: PlatformEvent) => {
            if (event instanceof ConnectionStartEvent) {
                const connectionEvent: ConnectionStartEvent = event as ConnectionStartEvent;
                uri = connectionEvent.Uri;
            }
        },
    });

    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    s.endpointId = Settings.SpeechTestEndpointId;

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            try {
                const res: sdk.SpeechRecognitionResult = p2;
                expect(res).not.toBeUndefined();
                expect(res.errorDetails).toBeUndefined();
                expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
                expect("What's the weather like?").toEqual(res.text);
                expect(uri).not.toBeUndefined();
                expect(uri.search(QueryParameterNames.DeploymentIdParamName + "=" + Settings.SpeechTestEndpointId)).not.toEqual(-1);
                expect(uri.search(QueryParameterNames.LanguageParamName)).toEqual(-1);

                done();
            } catch (error) {
                done.fail(error);
            }
        },
        (error: string) => {
            done.fail(error);
        });
});

test("Endpoint URL With Parameter Test", (done: jest.DoneCallback) => {
    let uri: string;

    Events.Instance.AttachListener({
        OnEvent: (event: PlatformEvent) => {
            if (event instanceof ConnectionStartEvent) {
                const connectionEvent: ConnectionStartEvent = event as ConnectionStartEvent;
                uri = connectionEvent.Uri;
            }
        },
    });

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromEndpoint(new URL("wss://fake.host.name?somequeryParam=Value"), "fakekey");
    objsToClose.push(s);

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            done.fail("bad URL connected?");
        },
        (error: string) => {
            try {
                expect(uri).not.toBeUndefined();
                // Make sure there's only a single ? in the URL.
                expect(uri.indexOf("?")).toEqual(uri.lastIndexOf("?"));
                done();
            } catch (error) {
                done.fail(error);
            }
        });
});

test("Connection Errors Propogate Async", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription("badKey", Settings.SpeechRegion);
    objsToClose.push(s);

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
            done();
        } catch (error) {
            done.fail(error);
        }
    };

    r.startContinuousRecognitionAsync();

});

test("Connection Errors Propogate Sync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription("badKey", Settings.SpeechRegion);
    objsToClose.push(s);

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    let doneCount: number = 0;
    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
            expect(e.errorDetails).toContain("1006");
            doneCount++;
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync((result: sdk.SpeechRecognitionResult) => {
        done.fail("RecognizeOnceAsync did not fail");
    }, (error: string) => {
        try {
            expect(error).toContain("1006");
        } catch (error) {
            done.fail(error);
        }
        doneCount++;
    });

    WaitForCondition(() => (doneCount === 2), done);

});

test("RecognizeOnce Bad Language", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);
    s.speechRecognitionLanguage = "BadLanguage";

    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);
    let doneCount: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
            expect(e.errorDetails).toContain("1006");
            doneCount++;
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync((result: sdk.SpeechRecognitionResult) => {
        done.fail("RecognizeOnceAsync did not fail");
    }, (error: string) => {
        try {
            expect(error).toContain("1006");
        } catch (error) {
            done.fail(error);
        }
        doneCount++;
    });

    WaitForCondition(() => (doneCount === 2), done);
});
