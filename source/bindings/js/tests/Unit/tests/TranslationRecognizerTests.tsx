// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { setTimeout } from "timers";

import * as sdk from "../../../microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener } from "../../../src/common.browser/Exports";
import { Events, EventType, ObjectDisposedError } from "../../../src/common/Exports";

import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

let objsToClose: any[];

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
    Events.Instance.AttachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, still looking for a way to get the test name to print that doesn't mean changing each test.
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
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
});

test("GetSourceLanguage", () => {
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).not.toBeNull();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_RecoLanguage]));
});

test("GetTargetLanguages", () => {
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.targetLanguages).not.toBeUndefined();
    expect(r.targetLanguages).not.toBeNull();
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages]));
});

test.skip("GetOutputVoiceNameNoSetting", () => {
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);
    expect(r.voiceName).not.toBeUndefined();
});

test("GetOutputVoiceName", () => {
    const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
    objsToClose.push(s);

    const voice: string = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";
    s.voiceName = voice;

    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    expect(r.voiceName).toEqual(voice);
});

test("GetParameters", () => {
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(r.properties).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_RecoLanguage, ""));

    // TODO this cannot be true, right? comparing an array with a string parameter???
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages));
});

test("RecognizeOnceAsync1", (done: jest.DoneCallback) => {
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
    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    r.startContinuousRecognitionAsync(() => {
        const end: number = Date.now() + 1000;

        WaitForCondition(() => {
            return end <= Date.now();
        }, () => {
            done();
        });
    }, (error: string) => done.fail(error));
});

test("StopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
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
            r.stopContinuousRecognitionAsync(() => {
                done();
            }, (error: string) => fail(error));
        });
    }, (error: string) => done.fail(error));
});

test("StartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
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

        r.stopContinuousRecognitionAsync(() => {
            done();
        }, (error: string) => done.fail(error));
    });
});

test("TranslateVoiceRoundTrip", (done: jest.DoneCallback) => {
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

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    done.fail(e.errorDetails);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(1);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            done.fail(error);
        }
    });

    r.startContinuousRecognitionAsync();

    WaitForCondition((): boolean => translationDone,
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
});

test("TranslateVoiceInvalidVoice", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
    objsToClose.push(s);

    s.voiceName = "Microsoft Server Speech Text to Speech Voice (de-DE, Hedda)";

    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        if (e.result.reason !== sdk.ResultReason.Canceled) {
            done.fail("Should have failed, instead got status");
        }
    });

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            expect(e.errorDetails).toEqual("Synthesis service failed with code:  - Could not identify the voice 'Microsoft Server Speech Text to Speech Voice (de-DE, Hedda))' for the text to speech service ");
            done();
        } catch (error) {
            done.fail(error);
        }
    });

    r.startContinuousRecognitionAsync();
});

test("TranslateVoiceUSToGerman", (done: jest.DoneCallback) => {
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

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    done.fail(e.errorDetails);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(1);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            done.fail(error);
        }
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
    WaitForCondition((): boolean => translationDone,
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
});

test.skip("MultiPhrase", (done: jest.DoneCallback) => {
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

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    done.fail(e.errorDetails);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(numPhrases);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            done.fail(error);
        }
    });

    r.startContinuousRecognitionAsync();

    WaitForCondition((): boolean => translationDone,
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
                const s2: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
                objsToClose.push(s2);
                s2.speechRecognitionLanguage = "de-DE";

                const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
                objsToClose.push(r2);

                let numEvents: number = 0;
                let speechEnded: boolean = false;

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
                            speechEnded = true;
                            break;
                        case sdk.CancellationReason.Error:
                            done.fail(e.errorDetails);
                            break;
                    }
                };

                r2.startContinuousRecognitionAsync(() => {
                    WaitForCondition(() => speechEnded,
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
}, 10000);

test("InitialSilenceTimeout (push)", (done: jest.DoneCallback) => {
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    p.write(bigFileBuffer.buffer);
    p.close();

    testInitialSilienceTimeout(config, done);
}, 10000);

test("InitialSilenceTimeout (File)", (done: jest.DoneCallback) => {

    const bigFileBuffer: Uint8Array = new Uint8Array(1024 * 1024);
    const bigFile: File = ByteBufferAudioFile.Load(bigFileBuffer.buffer);

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(bigFile);

    testInitialSilienceTimeout(config, done);
}, 10000);

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
            expect(Date.now()).toBeGreaterThan(startTime + 7500);

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

test.skip("emptyFile", (done: jest.DoneCallback) => {
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
    const s: sdk.SpeechTranslationConfig = BuildSpeechConfig();
    objsToClose.push(s);
    s.addTargetLanguage("de-DE");
    s.speechRecognitionLanguage = Settings.WaveFileLanguage;

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    objsToClose.push(r);

    expect(r instanceof sdk.Recognizer).toEqual(true);

});

test("Default mic is used when audio config is not specified.", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";
    s.addTargetLanguage("en-US");

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
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

test("Connection Errors Propogate Async", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription("badKey", Settings.SpeechRegion);
    objsToClose.push(s);
    s.addTargetLanguage("en-US");

    const r: sdk.TranslationRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
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

test.only("RecognizeOnce Bad Language", (done: jest.DoneCallback) => {
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
