//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { setTimeout } from "timers";

import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener } from "../../../../../source/bindings/js/src/common.browser/Exports";
import { Events, EventType } from "../../../../../source/bindings/js/src/common/Exports";

import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
    Events.Instance.AttachListener(new ConsoleLoggingListener(EventType.Debug));
});

// Test cases are run linerally, still looking for a way to get the test name to print that doesn't mean changing each test.
beforeEach(() => {
    // tslint:disable-next-line:no-console
    console.info("---------------------------------------Starting test case-----------------------------------");
    errorText = undefined;
});

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let eventIdentifier: number;
let errorText: string;

test("TranslationRecognizerMicrophone", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

test("TranslationRecognizerWavFile", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

test("GetSourceLanguage", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("en-us");

    s.speechRecognitionLanguage = "en-us";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r.speechRecognitionLanguage).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).not.toBeNull();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_RecoLanguage]));

    r.close();
    s.close();
});

test("GetTargetLanguages", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("en-US");

    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r.targetLanguages).not.toBeUndefined();
    expect(r.targetLanguages).not.toBeNull();
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_RecoLanguage]));

    r.close();
    s.close();
});

test.skip("GetOutputVoiceNameNoSetting", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("en-US");

    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r.voiceName).not.toBeUndefined();

    r.close();
    s.close();
});

test("GetOutputVoiceName", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("en-US");

    s.speechRecognitionLanguage = "en-US";

    const voice: string = "de-DE-Katja";
    s.voiceName = voice;

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r.voiceName).toEqual(voice);

    r.close();
    s.close();
});

test("GetParameters", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r.properties).not.toBeUndefined();
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_RecoLanguage, ""));

    // TODO this cannot be true, right? comparing an array with a string parameter???
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationToLanguages));

    r.close();
    s.close();
});

test("RecognizeOnceAsync1", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);
    s.addTargetLanguage("de-DE");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            if (e.result.reason === sdk.ResultReason.Canceled) {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail(e.result.reason);
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.recognizeOnceAsync(
        (res: sdk.TranslationRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
            expect(res.translations.get("de", undefined) !== undefined).toEqual(true);
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
            expect(res.text).toEqual("What's the weather like?");
            expect(errorText).toBeUndefined();

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

test("Translate Multiple Targets", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.addTargetLanguage("de-DE,en-US");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            if (e.result.reason === sdk.ResultReason.Canceled) {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail(e.result.reason);
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.recognizeOnceAsync(
        (res: sdk.TranslationRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
            expect("What's the weather like?").toEqual(res.translations.get("en", ""));
            expect(errorText).toBeUndefined();

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            r.close();
            s.close();
            done();
            fail(error);

        });
});

test.skip("RecognizeOnceAsync_badStream", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.recognizeOnceAsync(
        (res: sdk.TranslationRecognitionResult) => {
            fail("Should have hit an error");
        },
        (error: string) => {
            r.close();
            s.close();
            done();
        });
});

test("Validate Event Ordering", (done: jest.DoneCallback) => {
    const SpeechStartDetectedEvent = "SpeechStartDetectedEvent";
    const SpeechEndDetectedEvent = "SpeechEndDetectedEvent";
    const SessionStartedEvent = "SessionStartedEvent";
    const SessionStoppedEvent = "SessionStoppedEvent";
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

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

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            if (e.result.reason === sdk.ResultReason.Canceled) {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail(e.result.reason);
            }
        } catch (error) {
            errorText += error;
        }
    });

    // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeOnceAsync() call returns?!
    //      this is why below SessionStoppedEvent checks are conditional
    r.recognizeOnceAsync((res: sdk.TranslationRecognitionResult) => {
        expect(res).not.toBeUndefined();
        expect(res.errorDetails).toBeUndefined();
        expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.TranslatedSpeech]);

        expect(res.translations.get("en", "")).toEqual("What's the weather like?");

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
        expect(errorText).toBeUndefined();
        r.close();
        s.close();
        done();

    }, (error: string) => {
        fail(error);
        r.close();
        s.close();
        done();
    });
});

test("StartContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.startContinuousRecognitionAsync(() => {
        const end: number = Date.now() + 1000;

        WaitForCondition(() => {
            return end <= Date.now();
        }, () => {
            r.close();
            s.close();
            done();
        });
    }, (error: string) => fail(error));
});

test("StopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);
    r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
        try {
            expect(e.errorDetails).toBeUndefined();
            expect(e.reason).not.toEqual(sdk.CancellationReason.Error);
        } catch (error) {
            errorText += error;
        }
    };
    r.startContinuousRecognitionAsync(() => {
        const end: number = Date.now() + 1000;

        WaitForCondition(() => {
            return end <= Date.now();
        }, () => {
            r.stopContinuousRecognitionAsync(() => {
                r.close();
                s.close();
                expect(errorText).toBeUndefined();
                done();
            }, (error: string) => fail(error));
        });
    }, (error: string) => fail(error));
});

test("StartStopContinuousRecognitionAsync", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const rEvents: { [id: string]: string; } = {};

    r.recognized = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs) => {
        const result: string = e.result.translations.get("en", "");
        rEvents["Result@" + Date.now()] = result;
    });

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    const now: number = Date.now();

    WaitForCondition((): boolean => {
        return Object.keys(rEvents).length > 0;
    }, () => {
        expect(rEvents[Object.keys(rEvents)[0]]).toEqual("What's the weather like?");

        r.stopContinuousRecognitionAsync(() => {
            r.close();
            s.close();
            done();
        }, (error: string) => fail(error));
    });
});

test("TranslateVoiceRoundTrip", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.voiceName = "en-US-Zira";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    let synthCount: number = 0;
    let synthFragmentCount: number = 0;

    const rEvents: { [id: number]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            switch (e.result.reason) {
                case sdk.ResultReason.Canceled:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(e.result.reason);
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
            errorText += error;
        }
    });
    const foo: sdk.TranslationRecognitionResult;

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(e);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(1);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    const now: number = Date.now();

    WaitForCondition((): boolean => translationDone,
        () => {
            r.stopContinuousRecognitionAsync(() => {
                r.close();

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

                const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(speechConfig, config);
                r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                    expect(speech.errorDetails).toBeUndefined();
                    expect(speech.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                    expect(speech.text).toEqual("What's the weather like?");
                    expect(errorText).toBeUndefined();
                    r2.close();
                    s.close();
                    done();
                }, (error: string) => fail(error));
            }, (error: string) => fail(error));
        });
});

test("TranslateVoiceInvalidVoice", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);
    s.speechRecognitionLanguage = "en-US";

    s.voiceName = "de-DE-Hedda)";
    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            if (e.result.reason !== sdk.ResultReason.Canceled) {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail("Should have failed, instead got status");
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            r.close();
            s.close();
            setTimeout(done, 1);

            expect(errorText).toBeUndefined();
            expect(e.errorDetails).toEqual("Synthesis service failed with code:  - Could not identify the voice 'de-DE-Hedda)' for the text to speech service ");
        } catch (error) {
            errorText += error;
        }
    });

    r.startContinuousRecognitionAsync();
});

test("TranslateVoiceUSToGerman", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    s.addTargetLanguage("de-DE");

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    s.voiceName = "de-DE-Hedda";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    let synthCount: number = 0;
    let synthFragmentCount: number = 0;

    const rEvents: { [id: number]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            switch (e.result.reason) {
                case sdk.ResultReason.Canceled:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(e.result.reason);
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
            errorText += error;
        }
    });

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(e);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(1);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.recognizing = (o: sdk.Recognizer, e: sdk.TranslationRecognitionEventArgs): void => {
        try {
            expect(e.result.reason).toEqual(sdk.ResultReason.TranslatingSpeech);
        } catch (error) {
            errorText += error;
        }
    };

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    WaitForCondition((): boolean => translationDone,
        () => {
            r.stopContinuousRecognitionAsync(() => {
                r.close();

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
                s2.speechRecognitionLanguage = "de-DE";

                const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
                r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                    setTimeout(done, 1);
                    expect(speech.errorDetails).toBeUndefined();
                    expect(speech.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
                    expect(speech.text).toEqual("Wie ist das Wetter?");
                    expect(errorText).toBeUndefined();
                    r2.close();
                    s.close();

                }, (error: string) => {
                    r2.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(error);
                });
            }, (error: string) => {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail(error);
            });
        });
});

test("MultiPhrase", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.addTargetLanguage("en-US");
    s.speechRecognitionLanguage = "en-US";
    s.voiceName = "en-US-Zira";

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
    let synthCount: number = 0;
    let synthFragmentCount: number = 0;

    const rEvents: { [id: number]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            switch (e.result.reason) {
                case sdk.ResultReason.Canceled:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(sdk.ResultReason[e.result.reason]);
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
            errorText += error;
        }
    });

    let translationDone: boolean = false;

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    r.close();
                    s.close();
                    setTimeout(done, 1);
                    fail(e);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(synthCount).toEqual(numPhrases);
                    translationDone = true;
                    break;
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.startContinuousRecognitionAsync();

    WaitForCondition((): boolean => translationDone,
        () => {
            r.stopContinuousRecognitionAsync(() => {
                r.close();

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

                const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
                let numEvents: number = 0;
                let speechEnded: boolean = false;

                r2.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
                    expect(e.result.text).toEqual(Settings.WaveFileText);
                    numEvents++;
                };

                r2.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
                    switch (e.reason) {
                        case sdk.CancellationReason.EndOfStream:
                            speechEnded = true;
                            break;
                        case sdk.CancellationReason.Error:
                            setTimeout(done, 1);
                            fail(e.errorDetails);
                            break;
                    }
                };

                r2.startContinuousRecognitionAsync(() => {
                    WaitForCondition(() => speechEnded,
                        () => {
                            r2.stopContinuousRecognitionAsync(() => {
                                r2.close();
                                s.close();
                                setTimeout(done, 1);
                                expect(numEvents).toEqual(numPhrases);
                                expect(errorText).toBeUndefined();
                            }, (error: string) => {
                                fail(error);
                            });
                        });
                },
                    (error: string) => {
                        setTimeout(done, 1);
                        fail(error);
                    });

            }, (error: string) => {
                setTimeout(done, 1);
                fail(error);
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
    s.voiceName = "en-US-Zira";

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationVoice)).toEqual("en-US-Zira");

    // Change them.
    s.speechRecognitionLanguage = "de-DE";
    s.setProperty("RandomProperty", Math.random.toString());
    s.voiceName = "de-DE-Hedda";

    // Validate no change.
    expect(r.speechRecognitionLanguage).toEqual("en-US");
    expect(r.properties.getProperty("RandomProperty")).toEqual(ranVal);
    expect(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationVoice)).toEqual("en-US-Zira");

});

test("InitialSilenceTimeout", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";
    s.addTargetLanguage("en-US");

    let p: sdk.PullAudioInputStream;

    p = sdk.AudioInputStream.createPullStream(
        {
            close: () => { return; },
            read: (buffer: ArrayBuffer): number => {
                return buffer.byteLength;
            },
        });

    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.TranslationRecognitionResult) => {
            const res: sdk.TranslationRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.text).toBeUndefined();

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
}, 15000);

test.skip("emptyFile", (done: jest.DoneCallback) => {

    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";
    s.addTargetLanguage("en-US");

    const blob: Blob[] = [];
    const f: File = new File(blob, "file.wav");

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    let oneCalled: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.TranslationRecognitionCanceledEventArgs): void => {
        try {
            expect(e.reason).toEqual(sdk.CancellationReason.Error);
            const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(e.result);
            expect(cancelDetails.reason).toEqual(sdk.CancellationReason.Error);

            if (true === oneCalled) {
                expect(errorText).toBeUndefined();
                done();
            } else {
                oneCalled = true;
            }
        } catch (error) {
            errorText += error;
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            setTimeout(done, 1);
            r.close();
            s.close();

            if (true === oneCalled) {
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

test("Translate Bad Language", (done: jest.DoneCallback) => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);
    s.addTargetLanguage("bo-GU");
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s, config);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisEventArgs) => {
        try {
            if (e.result.reason === sdk.ResultReason.Canceled) {
                r.close();
                s.close();
                setTimeout(done, 1);
                fail(e.result.reason);
            }
        } catch (error) {
            errorText += error;
        }
    });

    r.recognizeOnceAsync(
        (res: sdk.TranslationRecognitionResult) => {
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedSpeech]);
            expect(res.translations).toBeUndefined();
            expect(res.text).toEqual("What's the weather like?");
            expect(errorText).toBeUndefined();

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

test("Audio Config is optional", () => {
    const s: sdk.SpeechTranslationConfig = sdk.SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";
    s.addTargetLanguage("en-us");

    const r: sdk.TranslationRecognizer = new sdk.TranslationRecognizer(s);
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
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
