// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import * as sdk from "../microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener, WebsocketMessageAdapter } from "../src/common.browser/Exports";
import { ServiceRecognizerBase } from "../src/common.speech/Exports";
import { QueryParameterNames } from "../src/common.speech/QueryParameterNames";
import { ConnectionStartEvent } from "../src/common/Exports";
import { Events, EventType, PlatformEvent } from "../src/common/Exports";

import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

import * as fs from "fs";
import { setTimeout } from "timers";
import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import WaitForCondition from "./Utilities";

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let objsToClose: any[];

beforeAll(() => {
    // override inputs, if necessary
    Settings.LoadSettings();
    Events.instance.attachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, the only other mechanism to demark them in the output is to put a console line in each case and
// report the name.
beforeEach(() => {
    objsToClose = [];
    // tslint:disable-next-line:no-console
    console.info("---------------------------------------Starting test case-----------------------------------");
    // tslint:disable-next-line:no-console
    console.info("Start Time: " + new Date(Date.now()).toLocaleString());
});

afterEach(() => {
    // tslint:disable-next-line:no-console
    console.info("End Time: " + new Date(Date.now()).toLocaleString());
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

test("testSpeechRecognizer1", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: testSpeechRecognizer1");
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
    // tslint:disable-next-line:no-console
    console.info("Name: testGetLanguage1");
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.speechRecognitionLanguage).not.toBeNull();
});

test("testGetLanguage2", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: testGetLanguage2");
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
    // tslint:disable-next-line:no-console
    console.info("Name: testGetOutputFormatDefault");
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.outputFormat === sdk.OutputFormat.Simple);
});

test("testGetParameters", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: testGetParameters");
    const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.properties).not.toBeUndefined();
    // expect(r.language ==  r.properties.getProperty(RecognizerParameterNames.SpeechRecognitionLanguage));
    // expect(r.deploymentId == r.properties.getProperty(RecognizerParameterNames.SpeechMspeechConfigImpl// TODO: is this really the correct mapping?
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.endpointId === r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_EndpointId, null)); // todo: is this really the correct mapping?
});

describe.each([true, false])("Service based tests", (forceNodeWebSocket: boolean) => {

    beforeAll(() => {
        WebsocketMessageAdapter.forceNpmWebSocket = forceNodeWebSocket;
    });

    afterAll(() => {
        WebsocketMessageAdapter.forceNpmWebSocket = false;
    });

    test("testGetOutputFormatDetailed", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: testGetOutputFormatDetailed");

        const s: sdk.SpeechConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.outputFormat = sdk.OutputFormat.Detailed;

        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        expect(r.outputFormat === sdk.OutputFormat.Detailed);

        r.recognizeOnceAsync((result: sdk.SpeechRecognitionResult) => {
            expect(result).not.toBeUndefined();
            expect(result.text).toEqual(Settings.WaveFileText);

            done();
        }, (error: string) => {
            done.fail(error);
        });
    });

    describe("Counts Telemetry", () => {
        afterAll(() => {
            ServiceRecognizerBase.telemetryData = undefined;
        });

        test("RecognizeOnce", (done: jest.DoneCallback) => {
            // tslint:disable-next-line:no-console
            console.info("Name: RecognizeOnce");

            const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
            objsToClose.push(r);

            let telemetryEvents: number = 0;
            let sessionId: string;

            r.sessionStarted = (r: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
                sessionId = e.sessionId;
            };

            ServiceRecognizerBase.telemetryData = (json: string): void => {
                // Only record telemetry events from this session.
                if (json !== undefined &&
                    sessionId !== undefined &&
                    json.indexOf(sessionId) > 0) {
                    telemetryEvents++;
                }
            };

            r.recognizeOnceAsync(
                (p2: sdk.SpeechRecognitionResult) => {
                    try {
                        const res: sdk.SpeechRecognitionResult = p2;
                        expect(res).not.toBeUndefined();
                        expect(res.text).toEqual("What's the weather like?");
                        expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                        expect(telemetryEvents).toEqual(1);
                        done();
                    } catch (error) {
                        done.fail(error);
                    }

                },
                (error: string) => {
                    done.fail(error);
                });
        });
    });

    test("Event Tests (RecognizeOnce)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Event Tests (RecognizeOnce)");
        const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
        const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
        const SessionStartedEvent = "SessionStartedEvent";
        const SessionStoppedEvent = "SessionStoppedEvent";
        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        const eventsMap: { [id: string]: number; } = {};
        let eventIdentifier: number = 1;

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
                done.fail(error);
            });

    });

    test("Event Tests (Continuous)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Event Tests (Continuous)");
        const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
        const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
        const SessionStartedEvent = "SessionStartedEvent";
        const SessionStoppedEvent = "SessionStoppedEvent";
        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        let sessionStopped: boolean = false;

        const eventsMap: { [id: string]: number; } = {};
        let eventIdentifier: number = 1;

        r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
            eventsMap[Recognized] = eventIdentifier++;
        };

        r.recognizing = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[Recognizing + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[Recognizing] = now;
        };

        r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.NoError]);
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.EndOfStream]);
                eventsMap[Canceled] = eventIdentifier++;
            } catch (error) {
                done.fail(error);
            }
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
            try {
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
            } catch (error) {
                done.fail(error);
            }
        });
    }, 20000);

    describe("Disables Telemetry", () => {

        // Re-enable telemetry
        afterEach(() => sdk.Recognizer.enableTelemetry(true));

        test("testStopContinuousRecognitionAsyncWithoutTelemetry", (done: jest.DoneCallback) => {
            // tslint:disable-next-line:no-console
            console.info("Name: testStopContinuousRecognitionAsyncWithoutTelemetry");
            // start with telemetry disabled
            const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
            objsToClose.push(r);

            let eventDone: boolean = false;
            let canceled: boolean = false;
            let telemetryEvents: number = 0;

            // disable telemetry data
            sdk.Recognizer.enableTelemetry(false);

            ServiceRecognizerBase.telemetryData = (json: string): void => {
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
                            // since we disabled, there should be no telemetry
                            // event run through our handler
                            expect(telemetryEvents).toEqual(0);
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
    });

    test("testStopContinuousRecognitionAsyncWithTelemetry", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: testStopContinuousRecognitionAsyncWithTelemetry");
        // Now, the same test, but with telemetry enabled.
        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        let eventDone: boolean = false;
        let canceled: boolean = false;
        let telemetryEvents: number = 0;

        // enable telemetry data
        sdk.Recognizer.enableTelemetry(true);

        ServiceRecognizerBase.telemetryData = (json: string): void => {
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

    test("Close with no recognition", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Close with no recognition");
        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);
    });

    test("Config is copied on construction", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Config is copied on construction");

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
        // tslint:disable-next-line:no-console
        console.info("Name: PushStream4KNoDelay");
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
                done.fail(error);
            });
    });

    test("PushStream4KPostRecognizePush", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: PushStream4KPostRecognizePush");
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
                done.fail(error);
            });

        const sendSize: number = 4096;

        for (i = sendSize - 1; i < f.byteLength; i += sendSize) {
            p.write(f.slice(i - (sendSize - 1), i));
        }

        p.write(f.slice(i - (sendSize - 1), f.byteLength - 1));
        p.close();

    });

    test("PullStreamFullFill", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: PullStreamFullFill");
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
                done.fail(error);
            });
    });

    test("PullStreamHalfFill", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: PullStreamHalfFill");
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
                done.fail(error);
            });
    });

    test("InitialSilenceTimeout (pull)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialSilenceTimeout (pull)");
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
    }, 15000);

    test("InitialSilenceTimeout (push)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialSilenceTimeout (push)");
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

        p.write(bigFileBuffer.buffer);
        p.close();

        testInitialSilienceTimeout(config, done);
    }, 15000);

    test("InitialSilenceTimeout (File)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialSilenceTimeout (File)");

        const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
        const bigFile: File = ByteBufferAudioFile.Load(bigFileBuffer.buffer);

        const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(bigFile);

        testInitialSilienceTimeout(config, done);
    }, 15000);

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
                expect(Date.now()).toBeGreaterThanOrEqual(startTime + ((res.offset / 1e+4) / 2));

            },
            (error: string) => {
                fail(error);
            });

        WaitForCondition(() => (numReports === 2), () => {
            try {
                if (!!addedChecks) {
                    addedChecks();
                }
                done();
            } catch (error) {
                done.fail(error);
            }
        });
    };

    test.skip("InitialBabbleTimeout", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialBabbleTimeout");

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
                done.fail(error);
            });
    });

    test("emptyFile", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: emptyFile");
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
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);

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
                expect(sdk.CancellationReason[cancelDetails.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);

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
        // tslint:disable-next-line:no-console
        console.info("Name: PullStreamSendHalfTheFile");
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
        // tslint:disable-next-line:no-console
        console.info("Name: burst of silence");
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

        r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.NoError]);
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.EndOfStream]);
            } catch (error) {
                done.fail(error);
            }
        };
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
        // tslint:disable-next-line:no-console
        console.info("Name: ecognizeOnceAsync is async");
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
                try {
                    expect(e.result.errorDetails).toBeUndefined();
                    expect(e.result.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                    expect(e.result.text).toEqual(Settings.WaveFileText);
                    done();
                } catch (error) {
                    done.fail(error);
                }
            });
        };

        r.recognizeOnceAsync();

        expect(resultSeen).toEqual(false);
        postCall = true;
    });

    test("InitialSilenceTimeout Continous", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialSilenceTimeout Continous");
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
            // Since the pull stream above will always return an empty array, there should be
            // no other reason besides an error for cancel to hit.
            done.fail(e.errorDetails);
        };

        let passed: boolean = false;

        r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {

            const res: sdk.SpeechRecognitionResult = e.result;
            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);
            passed = true;
        };

        /* tslint:disable:no-empty */
        r.startContinuousRecognitionAsync(() => {
        },
            (error: string) => {
                done.fail(error);
            });

        WaitForCondition(() => passed, () => {
            r.stopContinuousRecognitionAsync(() => {
                done();
            }, (error: string) => done.fail(error));
        });

    }, 30000);

    test("Audio Config is optional", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Audio Config is optional");
        const s: sdk.SpeechConfig = BuildSpeechConfig();
        objsToClose.push(s);

        const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s);
        objsToClose.push(r);
        expect(r instanceof sdk.Recognizer).toEqual(true);
    });

    test("Default mic is used when audio config is not specified.", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Default mic is used when audio config is not specified.");
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
        // tslint:disable-next-line:no-console
        console.info("Name: Using disposed recognizer invokes error callbacks.");
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

        Events.instance.attachListener({
            onEvent: (event: PlatformEvent) => {
                if (event instanceof ConnectionStartEvent) {
                    const connectionEvent: ConnectionStartEvent = event as ConnectionStartEvent;
                    uri = connectionEvent.uri;
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
        // tslint:disable-next-line:no-console
        console.info("Name: Endpoint URL With Parameter Test");
        let uri: string;

        Events.instance.attachListener({
            onEvent: (event: PlatformEvent) => {
                if (event instanceof ConnectionStartEvent) {
                    const connectionEvent: ConnectionStartEvent = event as ConnectionStartEvent;
                    uri = connectionEvent.uri;
                }
            },
        });

        const s: sdk.SpeechConfig = sdk.SpeechConfig.fromEndpoint(new URL("wss://fake.host.name?somequeryParam=Value"), "fakekey");
        objsToClose.push(s);

        const r: sdk.SpeechRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        r.recognizeOnceAsync(
            (p2: sdk.SpeechRecognitionResult) => {
                try {
                    expect(uri).not.toBeUndefined();
                    // Make sure there's only a single ? in the URL.
                    expect(uri.indexOf("?")).toEqual(uri.lastIndexOf("?"));

                    expect(p2.errorDetails).not.toBeUndefined();
                    expect(sdk.ResultReason[p2.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.Canceled]);

                    const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(p2);
                    expect(sdk.CancellationReason[cancelDetails.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                    expect(sdk.CancellationErrorCode[cancelDetails.ErrorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                    done();
                } catch (error) {
                    done.fail(error);
                }
            });
    });

    test("Connection Errors Propogate Async", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Connection Errors Propogate Async");
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
        // tslint:disable-next-line:no-console
        console.info("Name: Connection Errors Propogate Sync");
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
            try {
                const e: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(result);
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.ErrorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                expect(e.errorDetails).toContain("1006");
                doneCount++;
            } catch (error) {
                done.fail(error);
            }
        });

        WaitForCondition(() => (doneCount === 2), done);

    });

    test("RecognizeOnce Bad Language", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: RecognizeOnce Bad Language");
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
            try {
                const e: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(result);
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.ErrorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                expect(e.errorDetails).toContain("1006");
                doneCount++;
            } catch (error) {
                done.fail(error);
            }
            WaitForCondition(() => (doneCount === 2), done);
        });
    });

    test("Silence After Speech", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Silence After Speech");
        // Pump valid speech and then silence until at least one speech end cycle hits.
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const bigFileBuffer: Uint8Array = new Uint8Array(32 * 1024 * 30); // ~30 seconds.
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
        const s: sdk.SpeechConfig = BuildSpeechConfig();
        objsToClose.push(s);

        p.write(WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile));
        p.write(bigFileBuffer.buffer);
        p.close();

        const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
        objsToClose.push(r);

        let speechRecognized: boolean = false;
        let noMatchCount: number = 0;
        let speechEnded: number = 0;
        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
            try {
                if (e.result.reason === sdk.ResultReason.RecognizedSpeech) {
                    expect(speechRecognized).toEqual(false);
                    speechRecognized = true;
                    expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
                    expect(e.result.text).toEqual("What's the weather like?");
                } else if (e.result.reason === sdk.ResultReason.NoMatch) {
                    expect(speechRecognized).toEqual(true);
                    noMatchCount++;
                }
            } catch (error) {
                done.fail(error);
            }
        };

        r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(e.reason).toEqual(sdk.CancellationReason.EndOfStream);
                canceled = true;
            } catch (error) {
                done.fail(error);
            }
        };

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs): void => {
            speechEnded++;
        };

        r.startContinuousRecognitionAsync(() => {
            WaitForCondition(() => (canceled && !inTurn), () => {
                r.stopContinuousRecognitionAsync(() => {
                    try {
                        expect(speechEnded).toEqual(noMatchCount);
                        expect(noMatchCount).toEqual(2);
                        done();
                    } catch (error) {
                        done.fail(error);
                    }
                }, (error: string) => {
                    done.fail(error);
                });
            });
        },
            (err: string) => {
                done.fail(err);
            });
    }, 30000);

    test("Silence Then Speech", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Silence Then Speech");
        // Pump valid speech and then silence until at least one speech end cycle hits.
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const bigFileBuffer: Uint8Array = new Uint8Array(32 * 1024 * 30); // ~30 seconds.
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
        const s: sdk.SpeechConfig = BuildSpeechConfig();
        objsToClose.push(s);

        p.write(bigFileBuffer.buffer);
        p.write(WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile));
        p.close();

        const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
        objsToClose.push(r);

        let speechRecognized: boolean = false;
        let noMatchCount: number = 0;
        let speechEnded: number = 0;
        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
            try {
                if (e.result.reason === sdk.ResultReason.RecognizedSpeech) {
                    expect(speechRecognized).toEqual(false);
                    expect(noMatchCount).toBeGreaterThanOrEqual(1);
                    speechRecognized = true;
                    expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
                    expect(e.result.text).toEqual("What's the weather like?");
                } else if (e.result.reason === sdk.ResultReason.NoMatch) {
                    expect(speechRecognized).toEqual(false);
                    noMatchCount++;
                }
            } catch (error) {
                done.fail(error);
            }
        };

        r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(e.reason).toEqual(sdk.CancellationReason.EndOfStream);
                canceled = true;
            } catch (error) {
                done.fail(error);
            }
        };

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs): void => {
            speechEnded++;
        };

        r.startContinuousRecognitionAsync(() => {
            WaitForCondition(() => (canceled && !inTurn), () => {
                r.stopContinuousRecognitionAsync(() => {
                    try {
                        expect(speechEnded).toEqual(noMatchCount + 1);
                        expect(noMatchCount).toEqual(2);
                        done();
                    } catch (error) {
                        done.fail(error);
                    }
                }, (error: string) => {
                    done.fail(error);
                });
            });
        },
            (err: string) => {
                done.fail(err);
            });
    }, 35000);

    // Tests client reconnect after speech timeouts.
    test.skip("Reconnect After timeout", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Reconnect After timeout");
        // Pump valid speech and then silence until at least one speech end cycle hits.
        const fileBuffer: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);

        let p: sdk.PullAudioInputStream;
        const bigFileBuffer: Uint8Array = new Uint8Array(32 * 1024 * 30); // ~30 seconds.
        let s: sdk.SpeechConfig;
        if (undefined === Settings.SpeechTimeoutEndpoint || undefined === Settings.SpeechTimeoutKey) {
            // tslint:disable-next-line:no-console
            console.warn("Running timeout test against production, this will be very slow...");
            s = BuildSpeechConfig();
        } else {
            s = sdk.SpeechConfig.fromEndpoint(new URL(Settings.SpeechTimeoutEndpoint), Settings.SpeechTimeoutKey);
        }
        objsToClose.push(s);

        let pumpSilence: boolean = false;
        let bytesSent: number = 0;
        const targetLoops: number = 250;

        // Pump the audio from the wave file specified with 1 second silence between iterations indefinetly.
        p = sdk.AudioInputStream.createPullStream(
            {
                close: () => { return; },
                read: (buffer: ArrayBuffer): number => {
                    if (pumpSilence) {
                        bytesSent += buffer.byteLength;
                        if (bytesSent >= 8000) {
                            bytesSent = 0;
                            pumpSilence = false;
                        }
                        return buffer.byteLength;
                    } else {
                        const copyArray: Uint8Array = new Uint8Array(buffer);
                        const start: number = bytesSent;
                        const end: number = buffer.byteLength > (fileBuffer.byteLength - bytesSent) ? (fileBuffer.byteLength - 1) : (bytesSent + buffer.byteLength - 1);
                        copyArray.set(new Uint8Array(fileBuffer.slice(start, end)));
                        const readyToSend: number = (end - start) + 1;
                        bytesSent += readyToSend;

                        if (readyToSend < buffer.byteLength) {
                            bytesSent = 0;
                            pumpSilence = true;
                        }

                        return readyToSend;
                    }

                },
            });

        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

        const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
        objsToClose.push(r);

        let speechEnded: number = 0;
        let lastOffset: number = 0;
        let recogCount: number = 0;
        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
            try {
                expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
                expect(e.offset).toBeGreaterThanOrEqual(lastOffset);
                lastOffset = e.offset;

                // If there is silence exactly at the moment of disconnect, an extra speech.phrase with text ="" is returned just before the
                // connection is disconnected.
                if ("" !== e.result.text) {
                    expect(e.result.text).toEqual(Settings.WaveFileText);
                }
                if (recogCount++ > targetLoops) {
                    p.close();
                }

            } catch (error) {
                done.fail(error);
            }
        };

        r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs): void => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.EndOfStream]);
                canceled = true;
            } catch (error) {
                done.fail(error);
            }
        };

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs): void => {
            speechEnded++;
        };

        r.startContinuousRecognitionAsync(() => {
            WaitForCondition(() => (canceled && !inTurn), () => {
                r.stopContinuousRecognitionAsync(() => {
                    try {
                        expect(speechEnded).toEqual(1);
                        done();
                    } catch (error) {
                        done.fail(error);
                    }
                }, (error: string) => {
                    done.fail(error);
                });
            });
        },
            (err: string) => {
                done.fail(err);
            });
    }, 35000);
});

test("Push Stream Async", (done: jest.DoneCallback) => {
    // tslint:disable-next-line:no-console
    console.info("Name: Push Stream Async");

    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    fs.createReadStream(Settings.WaveFile).on("data", (buffer: Buffer) => {
        p.write(buffer.buffer);
    }).on("end", () => {
        p.close();
    });

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.canceled = (r: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        done.fail(e.errorDetails);
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.text).toEqual("What's the weather like?");
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("Bad DataType for PushStreams results in error", (done: jest.DoneCallback) => {
    // tslint:disable-next-line:no-console
    console.info("Name: Bad DataType for PushStreams results in error");

    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    // Wrong data type for ReadStreams
    fs.createReadStream(Settings.WaveFile).on("data", (buffer: ArrayBuffer) => {
        p.write(buffer);
    }).on("end", () => {
        p.close();
    });

    const r: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.canceled = (r: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        try {
            expect(e.errorDetails).not.toBeUndefined();
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.RuntimeError]);
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;
            try {
                expect(res).not.toBeUndefined();
                expect(res.errorDetails).not.toBeUndefined();
                expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.Canceled]);
                done();
            } catch (error) {
                done.fail(error);
            }
        },
        (error: string) => {
            done.fail(error);
        });
});
