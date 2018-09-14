//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { setTimeout } from "timers";
import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { RecognitionEventType, SessionEventType } from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

let eventIdentifier: number;

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
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationFromLanguage]));

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
    expect(r.targetLanguages[0]).toEqual(r.properties.getProperty(sdk.PropertyId[sdk.PropertyId.SpeechServiceConnection_TranslationFromLanguage]));

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
    expect(r.speechRecognitionLanguage).toEqual(r.properties.getProperty(sdk.PropertyId.SpeechServiceConnection_TranslationFromLanguage, ""));

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

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.reason);
        }
    });

    r.recognizeOnceAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect(res.translations.has("de")).toEqual(true);
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            fail(error);

            r.close();
            s.close();
            done();
        });
}, 10000);

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

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.reason);
        }
    });

    r.recognizeOnceAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
            expect("What's the weather like?").toEqual(res.translations.get("en", ""));

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            fail(error);

            r.close();
            s.close();
            done();
        });
}, 10000);

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
        (res: sdk.TranslationTextResult) => {
            fail("Should have hit an error");
        },
        (error: string) => {
            r.close();
            s.close();
            done();
        });
}, 10000);

test("Validate Event Ordering", (done: jest.DoneCallback) => {
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

    r.recognized = (o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
        eventsMap[Recognized] = eventIdentifier++;
    };

    r.recognizing = (o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Recognizing + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[Recognizing] = now;
    };

    r.canceled = (o: sdk.Recognizer, e: sdk.TranslationTextResultCanceledEventArgs) => {
        eventsMap[Canceled] = eventIdentifier++;

    };

    // TODO eventType should be renamed and be a function getEventType()
    r.speechStartDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[RecognitionEventType.SpeechStartDetectedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[RecognitionEventType.SpeechStartDetectedEvent.toString()] = now;
    };
    r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[RecognitionEventType.SpeechEndDetectedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[RecognitionEventType.SpeechEndDetectedEvent.toString()] = now;
    };

    r.sessionStarted = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + ":" + SessionEventType.SessionStartedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[Session + ":" + SessionEventType.SessionStartedEvent.toPrecision()] = now;
    };
    r.sessionStopped = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + ":" + SessionEventType.SessionStoppedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[Session + ":" + SessionEventType.SessionStoppedEvent.toPrecision()] = now;
    };

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.reason);
        }
    });

    // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeOnceAsync() call returns?!
    //      this is why below SessionStoppedEvent checks are conditional
    r.recognizeOnceAsync((res: sdk.TranslationTextResult) => {

        expect(res).not.toBeUndefined();
        expect(res.translations.get("en", "")).toEqual("What's the weather like?");

        // session events are first and last event
        const LAST_RECORDED_EVENT_ID: number = eventIdentifier;
        expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);
        expect(FIRST_EVENT_ID).toEqual(eventsMap[Session + ":" + sdk.SessionEventType.SessionStartedEvent.toString()]);

        if (Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap[Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }

        // end events come after start events.
        if (Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(eventsMap[Session + ":" + sdk.SessionEventType.SessionStartedEvent.toString()]).toBeLessThan(eventsMap[Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }

        expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

        // recognition events come after session start but before session end events
        expect(eventsMap[Session + ":" + sdk.SessionEventType.SessionStartedEvent.toString()]).toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

        if (Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]).toBeLessThan(eventsMap[Session + ":" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        expect(eventsMap[Recognizing]).toBeLessThan(eventsMap[Recognized]);

        // make sure events we don't expect, don't get raised
        expect(Canceled in eventsMap).toEqual(false);
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
        const start: number = Date.now();

        done();
        r.close();
        s.close();
    }, (error: string) => {
        r.close();
        s.close();
        fail(error);
    });
}, 10000);

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

    r.startContinuousRecognitionAsync(() => {
        const end: number = Date.now() + 1000;

        WaitForCondition(() => {
            return end <= Date.now();
        }, () => {
            r.stopContinuousRecognitionAsync(() => {
                r.close();
                s.close();
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

    r.recognized = ((o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
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

    const rEvents: { [id: string]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        const result: ArrayBuffer = e.result.audio;
        rEvents["Result@" + Date.now()] = result;
    });

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    const now: number = Date.now();

    WaitForCondition((): boolean => {
        return Object.keys(rEvents).length > 0;
    }, () => {
        r.stopContinuousRecognitionAsync(() => {
            r.close();
            const result: ArrayBuffer = rEvents[Object.keys(rEvents)[0]];

            const inputStream: File = ByteBufferAudioFile.Load(result);
            const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(inputStream);
            const speechConfig: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

            const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(speechConfig, config);
            r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                expect(speech.text).toEqual("What's the weather like?");
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

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason !== sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail("Should have failed, instead got status");
        }
    });

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationTextResultCanceledEventArgs) => {
        r.close();
        s.close();
        setTimeout(() => done(), 1);

        expect(e.errorDetails).toEqual("Synthesis service failed with code:  - Could not identify the voice 'de-DE-Hedda)' for the text to speech service ");
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

    const rEvents: { [id: string]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.reason);
        }

        const result: ArrayBuffer = e.result.audio;
        rEvents["Result@" + Date.now()] = result;
    });

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationTextResultCanceledEventArgs) => {
        r.close();
        s.close();
        setTimeout(() => done(), 1);
        fail(e);
    });

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    const now: number = Date.now();

    WaitForCondition((): boolean => {
        return Object.keys(rEvents).length > 0;
    }, () => {
        r.stopContinuousRecognitionAsync(() => {
            r.close();
            const result: ArrayBuffer = rEvents[Object.keys(rEvents)[0]];

            const inputStream: File = ByteBufferAudioFile.Load(result);
            const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(inputStream);
            const s2: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
            s2.speechRecognitionLanguage = "de-DE";

            const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
            r2.recognizeOnceAsync((speech: sdk.SpeechRecognitionResult) => {
                expect(speech.text).toEqual("Wie ist das Wetter?");
                r2.close();
                s.close();
                done();
            }, (error: string) => {
                r2.close();
                s.close();
                setTimeout(() => done(), 1);
                fail(error);
            });
        }, (error: string) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(error);
        });
    });
});

// TODO: Need a shorter file to use.
test.skip("MultiPhrase", (done: jest.DoneCallback) => {
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

    const rEvents: { [id: string]: ArrayBuffer; } = {};

    r.synthesizing = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.reason === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.reason);
        }

        const result: ArrayBuffer = e.result.audio;
        rEvents["Result@" + Date.now()] = result;
    });

    r.canceled = ((o: sdk.Recognizer, e: sdk.TranslationTextResultCanceledEventArgs) => {
        r.close();
        s.close();
        setTimeout(() => done(), 1);
        fail(e);
    });

    r.startContinuousRecognitionAsync();

    // wait until we get at least on final result
    const now: number = Date.now();

    WaitForCondition((): boolean => {
        return Object.keys(rEvents).length > 5;
    }, () => {
        r.stopContinuousRecognitionAsync(() => {
            r.close();

            let byteCount: number = 0;
            Object.keys(rEvents).forEach((value: string, index: number, array: string[]) => {
                byteCount += rEvents[value].byteLength;
            });

            const result: Uint8Array = new Uint8Array(byteCount);

            byteCount = 0;
            Object.keys(rEvents).forEach((value: string, index: number, array: string[]) => {
                result.set(new Uint8Array(rEvents[value]), byteCount);
                byteCount += rEvents[value].byteLength;
            });

            const inputStream: File = ByteBufferAudioFile.Load(result);
            const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(inputStream);
            const s2: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

            const r2: sdk.SpeechRecognizer = new sdk.SpeechRecognizer(s2, config);
            let constResult: string = "";
            let numEvents: number = 0;

            r2.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
                constResult += e.result.text + " ";
                numEvents++;
            };

            r2.startContinuousRecognitionAsync(() => {
                WaitForCondition(() => (numEvents > 4), () => {
                    r2.stopContinuousRecognitionAsync();
                    r2.close();
                    s.close();
                    setTimeout(() => done(), 1);
                    expect(constResult).toEqual("Skills and abilities Batman has no inherent super powers. He relies on his own scientific knowledge detective skills and athletic prowess. In the stories Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe. ");
                });
            },
                (error: string) => {
                    r2.close();
                    s.close();
                    setTimeout(() => done(), 1);
                    fail(error);
                });

        }, (error: string) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
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
