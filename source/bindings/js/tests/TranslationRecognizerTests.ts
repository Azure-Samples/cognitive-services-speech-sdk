// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as sdk from "../microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener, WebsocketMessageAdapter } from "../src/common.browser/Exports";
import { Events, EventType, ObjectDisposedError } from "../src/common/Exports";

import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

import * as fs from "fs";

let objsToClose: any[];

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
    Events.instance.attachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, still looking for a way to get the test name to print that doesn't mean changing each test.
beforeEach(() => {
    objsToClose = [];
    // tslint:disable-next-line:no-console
    console.info("---------------------------------------Starting test case-----------------------------------");
    // tslint:disable-next-line:no-console
    console.info("Sart Time: " + new Date(Date.now()).toLocaleString());
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

const BuildRecognizerFromWaveFile: (speechConfig?: sdk.SpeechTranslationConfig) => sdk.TranslationRecognizer = (speechConfig?: sdk.SpeechTranslationConfig): sdk.TranslationRecognizer => {

    let s: sdk.SpeechTranslationConfig = speechConfig;
    if (s === undefined) {
        s = BuildSpeechConfig();
        // Since we're not going to return it, mark it for closure.
        objsToClose.push(s);
    }

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = Settings.WaveFileLanguage;
    if (s.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_RecoLanguage]) === undefined) {
        s.speechRecognitionLanguage = language;
    }
    s.addTargetLanguage("de-DE");

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    return r;
};

const BuildSpeechConfig: () => sdk.SpeechTranslationConfig = (): sdk.SpeechTranslationConfig => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    return s;
};

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let eventIdentifier: number;

test("TranslationRecognizerMicrophone", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: TranslationRecognizerMicrophone");

    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    objsToClose.push(s);
    expect(s).not.toBeUndefined();
    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    objsToClose.push(r);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer).toEqual(true);
});

test("TranslationRecognizerWavFile", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: TranslationRecognizerWavFile");
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
});

test("GetSourceLanguage", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: GetSourceLanguage");
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).not.toBeNull();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_RecoLanguage]));
});

test("GetTargetLanguages", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: GetTargetLanguages");
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.targetLanguages).not.toBeUndefined();
    expect(r.targetLanguages).not.toBeNull();
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages]));
});

test.skip("GetOutputVoiceNameNoSetting", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: GetOutputVoiceNameNoSetting");
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
    expect(r.voiceName).not.toBeUndefined();
});

test("GetOutputVoiceName", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: GetOutputVoiceName");
    const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const voice: string = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";
    s.voiceName = voice;

    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    expect(r.voiceName).toEqual(voice);
});

test("GetParameters", () => {
    // tslint:disable-next-line:no-console
    console.info("Name: GetParameters");
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.properties).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_RecoLanguage, ""));

    // TODO this cannot be true, right? comparing an array with a string parameter???
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages));
});

describe.each([true, false])("Service based tests", (forceNodeWebSocket: boolean) => {

    beforeEach(() => {
        // tslint:disable-next-line:no-console
        console.info("forceNodeWebSocket: " + forceNodeWebSocket);
        WebsocketMessageAdapter.forceNpmWebSocket = forceNodeWebSocket;
    });
    afterAll(() => {
        WebsocketMessageAdapter.forceNpmWebSocket = false;
    });

    test("RecognizeOnceAsync1", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: RecognizeOnceAsync1");
        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        r.recognizeOnceAsync(
            (res: sdk.TranslationRecognitionResult) => {
                expect(res).not.toBeUndefined();
                expect(res.errorDetails).toBeUndefined();
                expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
                expect(res.translations.get("de", undefined) !== undefined).toEqual(true);
                expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
                expect(res.text).toEqual("What's the weather like?");

                done();
            },
            (error: string) => {
                done.fail(error);
            });
    });

    test("Translate Multiple Targets", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Translate Multiple Targets");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);
        s.addTargetLanguage("en-US");

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        r.recognizeOnceAsync(
            (res: sdk.TranslationRecognitionResult) => {
                expect(res).not.toBeUndefined();
                expect(res.errorDetails).toBeUndefined();
                expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
                expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
                expect("What's the weather like?").toEqual(res.translations.get("en", ""));
                done();
            },
            (error: string) => {
                done.fail(error);
            });
    });

    test("Validate Event Ordering", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Validate Event Ordering");
        const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
        const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
        const SessionStartedEvent = "SessionStartedEvent";
        const SessionStoppedEvent = "SessionStoppedEvent";

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        const eventsMap: { [id: string]: number; } = {};
        eventIdentifier = 1;

        r.recognized = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
            eventsMap[Recognized] = eventIdentifier++;
        };

        r.recognizing = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[Recognizing + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[Recognizing] = now;
        };

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            eventsMap[Canceled] = eventIdentifier++;
        };

        // TODO eventType should be renamed and be a function getEventType()
        r.speechStartDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[SpeechStartDetectedEvent + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[SpeechStartDetectedEvent] = now;
        };
        r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[SpeechEndDetectedEvent + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[SpeechEndDetectedEvent] = now;
        };

        r.sessionStarted = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[Session + ":" + SessionStartedEvent + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[Session + ":" + SessionStartedEvent] = now;
        };
        r.sessionStopped = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
            const now: number = eventIdentifier++;
            eventsMap[Session + ":" + SessionStoppedEvent + "-" + Date.now().toPrecision(4)] = now;
            eventsMap[Session + ":" + SessionStoppedEvent] = now;
        };

        // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeOnceAsync() call returns?!
        //      this is why below SessionStoppedEvent checks are conditional
        r.recognizeOnceAsync((res: sdk.TranslationRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);

            expect(res.translations.get("de", "No Translation")).toEqual("Wie ist das Wetter?");

            // session events are first and last event
            const LAST_RECORDED_EVENT_ID: number = eventIdentifier;
            expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);
            expect(FIRST_EVENT_ID).toEqual(eventsMap[Session + ":" + SessionStartedEvent]);

            if (Session + ":" + SessionStoppedEvent in eventsMap) {
                expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap[Session + ":" + SessionStoppedEvent]);
            }

            // end events come after start events.
            if (Session + ":" + SessionStoppedEvent in eventsMap) {
                expect(eventsMap[Session + ":" + SessionStartedEvent]).toBeLessThan(eventsMap[Session + ":" + SessionStoppedEvent]);
            }

            expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[SpeechStartDetectedEvent]);

            // recognition events come after session start but before session end events
            expect(eventsMap[Session + ":" + SessionStartedEvent]).toBeLessThan(eventsMap[SpeechStartDetectedEvent]);

            if (Session + ":" + SessionStoppedEvent in eventsMap) {
                expect(eventsMap[SpeechEndDetectedEvent]).toBeLessThan(eventsMap[Session + ":" + SessionStoppedEvent]);
            }

            // there is no partial result reported after the final result
            // (and check that we have intermediate and final results recorded)
            expect(eventsMap[Recognizing]).toBeLessThan(eventsMap[Recognized]);

            // make sure events we don't expect, don't get raised
            expect(Canceled in eventsMap).toEqual(false);
            done();

        }, (error: string) => {
            done.fail(error);
        });
    });

    test("StartContinuousRecognitionAsync", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: StartContinuousRecognitionAsync");
        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        r.startContinuousRecognitionAsync(() => {

            // Just long enough to start the connection, but not as long as recognition takes.
            const end: number = Date.now() + 1000;

            WaitForCondition(() => {
                return end <= Date.now();
            }, () => {
                r.stopContinuousRecognitionAsync(() => {
                    done();
                }, (error: string) => done.fail(error));
            });
        }, (error: string) => done.fail(error));
    });

    test("StopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: StopContinuousRecognitionAsync");
        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(e.reason).not.toEqual(sdk.CancellationReason.Error);
            } catch (error) {
                done.fail(error);
            }
        };
        r.startContinuousRecognitionAsync(() => {
            const end: number = Date.now() + 1000;

            WaitForCondition(() => {
                return end <= Date.now();
            }, () => {
                r.stopContinuousRecognitionAsync(() => done(), (error: string) => done.fail(error));
            });
        }, (error: string) => done.fail(error));
    });

    test("StartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: StartStopContinuousRecognitionAsync");
        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
        objsToClose.push(r);

        const rEvents: { [id: string]: string; } = {};

        r.recognized = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
            const result: string = e.result.translations.get("de", "");
            rEvents["Result@" + Date.now()] = result;
        });

        r.startContinuousRecognitionAsync();

        WaitForCondition((): boolean => {
            return Object.keys(rEvents).length > 0;
        }, () => {
            expect(rEvents[Object.keys(rEvents)[0]]).toEqual("Wie ist das Wetter?");

            r.stopContinuousRecognitionAsync(() => done(), (error: string) => done.fail(error));
        });
    });

    test("TranslateVoiceRoundTrip", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: TranslateVoiceRoundTrip");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.voiceName = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        let synthCount: number = 0;
        let synthFragmentCount: number = 0;

        const rEvents: { [id: number]: ArrayBuffer; } = {};

        r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
            switch (e.result.reason) {
                case sdk.ResultReason.Canceled:
                    done.fail(sdk.ResultReason[e.result.reason]);
                    break;
                case sdk.ResultReason.SynthesizingAudio:
                    const result: ArrayBuffer = e.result.audio;
                    rEvents[synthFragmentCount++] = result;
                    break;
                case sdk.ResultReason.SynthesizingAudioCompleted:
                    synthCount++;
                    break;
            }
        });

        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                switch (e.reason) {
                    case sdk.CancellationReason.Error:
                        done.fail(e.errorDetails);
                        break;
                    case sdk.CancellationReason.EndOfStream:
                        expect(synthCount).toEqual(1);
                        canceled = true;
                        break;
                }
            } catch (error) {
                done.fail(error);
            }
        });

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.startContinuousRecognitionAsync();

        WaitForCondition((): boolean => (canceled && !inTurn),
            () => {
                r.stopContinuousRecognitionAsync(() => {
                    let byteCount: number = 0;

                    for (let i: number = 0; i < synthFragmentCount; i++) {
                        byteCount += rEvents[i].byteLength;
                    }

                    const result: Uint8Array = new Uint8Array(byteCount);

                    byteCount = 0;
                    for (let i: number = 0; i < synthFragmentCount; i++) {
                        result.set(new Uint8Array(rEvents[i]), byteCount);
                        byteCount += rEvents[i].byteLength;
                    }

                    const inputStream: File = ByteBufferAudioFile.Load(result);
                    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(inputStream);
                    const speechConfig: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
                    objsToClose.push(speechConfig);
                    speechConfig.speechRecognitionLanguage = "de-DE";

                    const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(speechConfig, config);
                    objsToClose.push(r2);

                    r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                        expect(speech.errorDetails).toBeUndefined();
                        expect(speech.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                        expect(speech.text).toEqual("Wie ist das Wetter?");
                        done();
                    }, (error: string) => done.fail(error));
                }, (error: string) => done.fail(error));
            });
    }, 10000);

    test("TranslateVoiceInvalidVoice", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: TranslateVoiceInvalidVoice");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.voiceName = "Microsoft Server Speech Text to Speech Voice (BadVoice)";

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
            try {
                expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.Canceled]);
            } catch (error) {
                done.fail(error);
            }

        });

        let stopReco: boolean = false;
        let pass: boolean = false;

        r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                stopReco = true;
                if (!pass) {
                    expect(e.errorDetails).toEqual("Synthesis service failed with code:  - Could not identify the voice 'Microsoft Server Speech Text to Speech Voice (BadVoice)' for the text to speech service ");
                } else {
                    expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.EndOfStream]);
                }

                pass = true;
            } catch (error) {
                done.fail(error);
            }
        });

        r.startContinuousRecognitionAsync();

        WaitForCondition(() => stopReco, () => {
            r.stopContinuousRecognitionAsync(() => {
                if (pass) {
                    done();
                }
            });
        });
    });

    test("TranslateVoiceUSToGerman", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: TranslateVoiceUSToGerman");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.voiceName = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        let synthCount: number = 0;
        let synthFragmentCount: number = 0;

        const rEvents: { [id: number]: ArrayBuffer; } = {};

        r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
            try {
                switch (e.result.reason) {
                    case sdk.ResultReason.Canceled:
                        done.fail(sdk.ResultReason[e.result.reason]);
                        break;
                    case sdk.ResultReason.SynthesizingAudio:
                        const result: ArrayBuffer = e.result.audio;
                        rEvents[synthFragmentCount++] = result;
                        break;
                    case sdk.ResultReason.SynthesizingAudioCompleted:
                        synthCount++;
                        break;
                }
            } catch (error) {
                done.fail(error);
            }
        });

        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                switch (e.reason) {
                    case sdk.CancellationReason.Error:
                        done.fail(e.errorDetails);
                        break;
                    case sdk.CancellationReason.EndOfStream:
                        expect(synthCount).toEqual(1);
                        canceled = true;
                        break;
                }
            } catch (error) {
                done.fail(error);
            }
        });

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.recognizing = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs): void => {
            try {
                expect(e.result.reason).toEqual(sdk.ResultReason.TranslatingSpeech);
            } catch (error) {
                done.fail(error);
            }
        };

        r.startContinuousRecognitionAsync();

        // wait until we get at least on final result
        WaitForCondition((): boolean => (canceled && !inTurn),
            () => {
                r.stopContinuousRecognitionAsync(() => {
                    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();

                    for (let i: number = 0; i < synthFragmentCount; i++) {
                        p.write(rEvents[i]);
                    }
                    p.close();

                    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
                    const s2: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
                    objsToClose.push(s2);
                    s2.speechRecognitionLanguage = "de-DE";

                    const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
                    objsToClose.push(r2);

                    r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                        expect(speech.errorDetails).toBeUndefined();
                        expect(speech.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                        expect(speech.text).toEqual("Wie ist das Wetter?");
                        done();
                    }, (error: string) => {
                        done.fail(error);
                    });
                }, (error: string) => {
                    done.fail(error);
                });
            });
    }, 10000);

    test.skip("MultiPhrase", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: MultiPhrase");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.voiceName = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";
        s.addTargetLanguage("de-DE");
        s.speechRecognitionLanguage = Settings.WaveFileLanguage;

        const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile);
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
        const numPhrases: number = 3;

        for (let i: number = 0; i < 3; i++) {
            p.write(f);
        }

        p.close();

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        expect(r).not.toBeUndefined();
        expect(r instanceof sdk.Recognizer).toEqual(true);
        objsToClose.push(r);

        let synthCount: number = 0;
        let synthFragmentCount: number = 0;

        const rEvents: { [id: number]: ArrayBuffer; } = {};

        r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
            try {
                switch (e.result.reason) {
                    case sdk.ResultReason.Canceled:
                        done.fail(sdk.ResultReason[e.result.reason]);
                        break;
                    case sdk.ResultReason.SynthesizingAudio:
                        const result: ArrayBuffer = e.result.audio;
                        rEvents[synthFragmentCount++] = result;
                        break;
                    case sdk.ResultReason.SynthesizingAudioCompleted:
                        synthCount++;
                        break;
                }
            } catch (error) {
                done.fail(error);
            }
        });

        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                switch (e.reason) {
                    case sdk.CancellationReason.Error:
                        done.fail(e.errorDetails);
                        break;
                    case sdk.CancellationReason.EndOfStream:
                        expect(synthCount).toEqual(numPhrases);
                        canceled = true;
                        break;
                }
            } catch (error) {
                done.fail(error);
            }
        });

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.startContinuousRecognitionAsync();

        WaitForCondition((): boolean => (canceled && !inTurn),
            () => {
                r.stopContinuousRecognitionAsync(() => {
                    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();

                    for (let i: number = 0; i < synthFragmentCount; i++) {
                        p.write(rEvents[i]);
                    }
                    p.close();

                    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
                    const s2: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
                    objsToClose.push(s2);
                    s2.speechRecognitionLanguage = "de-DE";

                    const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
                    objsToClose.push(r2);

                    let numEvents: number = 0;
                    canceled = false;

                    r2.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
                        inTurn = true;
                    });

                    r2.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
                        inTurn = false;
                    });

                    r2.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
                        try {
                            expect(e.result.text).toEqual("Wie ist das Wetter?");
                            numEvents++;
                        } catch (error) {
                            done.fail(error);
                        }
                    };

                    r2.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
                        switch (e.reason) {
                            case sdk.CancellationReason.EndOfStream:
                                canceled = true;
                                break;
                            case sdk.CancellationReason.Error:
                                done.fail(e.errorDetails);
                                break;
                        }
                    };

                    r2.startContinuousRecognitionAsync(() => {
                        WaitForCondition(() => (canceled && !inTurn),
                            () => {
                                r2.stopContinuousRecognitionAsync(() => {
                                    expect(numEvents).toEqual(numPhrases);
                                    done();
                                }, (error: string) => {
                                    done.fail(error);
                                });
                            });
                    },
                        (error: string) => {
                            done.fail(error);
                        });

                }, (error: string) => {
                    done.fail(error);
                });
            });
    }, 45000);

    test("Config is copied on construction", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Config is copied on construction");
        const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        expect(s).not.toBeUndefined();
        s.speechRecognitionLanguage = "en-US";
        s.addTargetLanguage("en-US");

        const ranVal: string = Math.random().toString();
        s.setProperty("RandomProperty", ranVal);
        s.voiceName = "Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)";

        const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
        const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        expect(r).not.toBeUndefined();
        expect(r instanceof sdk.Recognizer);

        expect(r.speechRecognitionLanguage).toEqual("en-US");
        expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
        expect(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationVoice)).toEqual("Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)");

        // Change them.
        s.speechRecognitionLanguage = "de-DE";
        s.setProperty("RandomProperty", Math.random.toString());
        s.voiceName = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";

        // Validate no change.
        expect(r.speechRecognitionLanguage).toEqual("en-US");
        expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
        expect(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationVoice)).toEqual("Microsoft Server Speech Text to Speech Voice (en-US, ZiraRUS)");

    });

    test("InitialSilenceTimeout (pull)", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: InitialSilenceTimeout (pull)");
        let p: sdk.PullAudioInputStream;
        let bytesSent: number = 0;

        // To make sure we don't send a ton of extra data.
        // 5s * 16K * 2 * 1.25;
        // For reference, before the throttling was implemented, we sent 6-10x the required data.
        const expectedBytesSent: number = 15 * 16000 * 2 * 1.25;

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
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.addTargetLanguage("de-DE");
        s.speechRecognitionLanguage = "en-US";

        // To validate the data isn't sent too fast.
        const startTime: number = Date.now();

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        objsToClose.push(r);

        expect(r).not.toBeUndefined();
        expect(r instanceof sdk.Recognizer);

        let numReports: number = 0;

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            done.fail(e.errorDetails);
        };

        r.recognized = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
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
            (p2: sdk.TranslationRecognitionResult) => {
                const res: sdk.TranslationRecognitionResult = p2;
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

    test.skip("emptyFile", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: emptyFile");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        const blob: Blob[] = [];
        const f: File = new File(blob, "file.wav");

        const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        objsToClose.push(r);
        expect(r).not.toBeUndefined();
        expect(r instanceof sdk.Recognizer);
        let oneCalled: boolean = false;

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
            try {
                expect(e.reason).toEqual(sdk.CancellationReason.Error);
                const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(e.result);
                expect(cancelDetails.reason).toEqual(sdk.CancellationReason.Error);

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

    test("Translate Bad Language", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Translate Bad Language");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.addTargetLanguage("bo-GU");

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        expect(r).not.toBeUndefined();

        expect(r instanceof sdk.Recognizer).toEqual(true);

        r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
            try {
                if (e.result.reason === sdk.ResultReason.Canceled) {
                    done.fail(sdk.ResultReason[e.result.reason]);
                }
            } catch (error) {
                done.fail(error);
            }
        });

        r.recognizeOnceAsync(
            (res: sdk.TranslationRecognitionResult) => {
                expect(res).not.toBeUndefined();
                expect(res.errorDetails).toContain("400036");
                expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
                expect(res.translations).toBeUndefined();
                expect(res.text).toEqual("What's the weather like?");
                done();
            },
            (error: string) => {
                done.fail(error);
            });
    });

    test("Audio Config is optional", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Audio Config is optional");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);
        s.addTargetLanguage("de-DE");
        s.speechRecognitionLanguage = Settings.WaveFileLanguage;

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
        objsToClose.push(r);

        expect(r instanceof sdk.Recognizer).toEqual(true);

    });

    test("Default mic is used when audio config is not specified.", () => {
        // tslint:disable-next-line:no-console
        console.info("Name: Default mic is used when audio config is not specified.");
        const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        expect(s).not.toBeUndefined();
        s.speechRecognitionLanguage = "en-US";
        s.addTargetLanguage("en-US");

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
        expect(r instanceof sdk.Recognizer).toEqual(true);
        // Node.js doesn't have a microphone natively. So we'll take the specific message that indicates that microphone init failed as evidence it was attempted.
        r.recognizeOnceAsync(() => fail("RecognizeOnceAsync returned success when it should have failed"),
            (error: string): void => {
                expect(error).not.toBeUndefined();
                expect(error).toEqual("Error: Browser does not support Web Audio API (AudioContext is not available).");
            });

        r.startContinuousRecognitionAsync(() => fail("startContinuousRecognitionAsync returned success when it should have failed"),
            (error: string): void => {
                expect(error).not.toBeUndefined();
                expect(error).toEqual("Error: Browser does not support Web Audio API (AudioContext is not available).");
            });
    });

    test("Connection Errors Propogate Async", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Connection Errors Propogate Async");
        const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription("badKey", Settings.SpeechRegion);
        objsToClose.push(s);
        s.addTargetLanguage("en-US");

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        let pass: boolean = false;
        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                pass = true;
            } catch (error) {
                done.fail(error);
            }
        };

        r.startContinuousRecognitionAsync();

        WaitForCondition(() => pass, () => r.stopContinuousRecognitionAsync(() => done(), (error: string) => done.fail(error)));
    });

    test("Connection Errors Propogate Sync", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Connection Errors Propogate Sync");
        const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription("badKey", Settings.SpeechRegion);
        objsToClose.push(s);
        s.addTargetLanguage("en-US");

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);

        let doneCount: number = 0;
        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                expect(e.errorDetails).toContain("1006");
                doneCount++;
            } catch (error) {
                done.fail(error);
            }
        };

        r.recognizeOnceAsync((result: sdk.TranslationRecognitionResult) => {
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

    test("RecognizeOnce Bad Language", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: RecognizeOnce Bad Language");
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);
        s.speechRecognitionLanguage = "BadLanguage";
        s.addTargetLanguage("en-US");

        const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
        objsToClose.push(r);
        let doneCount: number = 0;

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
                expect(e.errorDetails).toContain("1006");
                doneCount++;
            } catch (error) {
                done.fail(error);
            }
        };

        r.recognizeOnceAsync((result: sdk.TranslationRecognitionResult) => {
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

    test("Silence After Speech", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Silence After Speech");
        // Pump valid speech and then silence until at least one speech end cycle hits.
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const bigFileBuffer: Uint8Array = new Uint8Array(32 * 1024 * 30); // ~30 seconds.
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        s.addTargetLanguage("de-DE");
        s.speechRecognitionLanguage = "en-US";
        objsToClose.push(s);

        p.write(WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile));
        p.write(bigFileBuffer.buffer);
        p.close();

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        objsToClose.push(r);

        let speechRecognized: boolean = false;
        let noMatchCount: number = 0;
        let speechEnded: number = 0;

        r.recognized = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
            try {
                if (e.result.reason === sdk.ResultReason.TranslatedSpeech) {
                    expect(speechRecognized).toEqual(false);
                    speechRecognized = true;
                    expect(sdk.ResultReason[e.result.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
                    expect(e.result.text).toEqual("What's the weather like?");
                } else if (e.result.reason === sdk.ResultReason.NoMatch) {
                    expect(speechRecognized).toEqual(true);
                    noMatchCount++;
                }
            } catch (error) {
                done.fail(error);
            }
        };

        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.sessionStarted = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = true;
        });

        r.sessionStopped = ((s: sdk.Recognizer, e: sdk.SessionEventArgs): void => {
            inTurn = false;
        });

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
            try {
                expect(e.errorDetails).toBeUndefined();
                expect(e.reason).toEqual(sdk.CancellationReason.EndOfStream);
                canceled = true;
            } catch (error) {
                done.fail(error);
            }
        };

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
    }, 35000);

    test("Silence Then Speech", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Silence Then Speech");
        // Pump valid speech and then silence until at least one speech end cycle hits.
        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const bigFileBuffer: Uint8Array = new Uint8Array(32 * 1024 * 30); // ~30 seconds.
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);
        s.speechRecognitionLanguage = "en-US";
        s.addTargetLanguage("de-DE");

        p.write(bigFileBuffer.buffer);
        p.write(WaveFileAudioInput.LoadArrayFromFile(Settings.WaveFile));
        p.close();

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        objsToClose.push(r);

        let speechRecognized: boolean = false;
        let noMatchCount: number = 0;
        let speechEnded: number = 0;

        let canceled: boolean = false;
        let inTurn: boolean = false;

        r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
            try {
                switch (e.reason) {
                    case sdk.CancellationReason.Error:
                        done.fail(e.errorDetails);
                        break;
                    case sdk.CancellationReason.EndOfStream:
                        canceled = true;
                        break;
                }
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

        r.recognized = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
            try {
                const res: sdk.TranslationRecognitionResult = e.result;
                expect(res).not.toBeUndefined();
                if (res.reason === sdk.ResultReason.TranslatedSpeech) {
                    expect(speechRecognized).toEqual(false);
                    expect(noMatchCount).toBeGreaterThanOrEqual(1);
                    speechRecognized = true;
                    expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
                    expect(res.text).toEqual("What's the weather like?");
                } else if (res.reason === sdk.ResultReason.NoMatch) {
                    expect(speechRecognized).toEqual(false);
                    noMatchCount++;
                }
            } catch (error) {
                done.fail(error);
            }
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

    test("Bad DataType for PushStreams results in error", (done: jest.DoneCallback) => {
        // tslint:disable-next-line:no-console
        console.info("Name: Bad DataType for PushStreams results in error");

        const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
        objsToClose.push(s);

        s.addTargetLanguage("en-US");
        s.speechRecognitionLanguage = "en-US";

        const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
        const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

        // Wrong data type for ReadStreams
        fs.createReadStream(Settings.WaveFile).on("data", (buffer: ArrayBuffer) => {
            p.write(buffer);
        }).on("end", () => {
            p.close();
        });

        const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
        objsToClose.push(r);

        expect(r).not.toBeUndefined();
        expect(r instanceof sdk.Recognizer);

        r.canceled = (r: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
            try {
                expect(e.errorDetails).not.toBeUndefined();
                expect(e.errorDetails).toContain("ArrayBuffer");
                expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
                expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.RuntimeError]);
            } catch (error) {
                done.fail(error);
            }
        };

        r.recognizeOnceAsync(
            (p2: sdk.TranslationRecognitionResult) => {
                const res: sdk.TranslationRecognitionResult = p2;
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
});
