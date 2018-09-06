//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { setTimeout } from "timers";
import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { ByteBufferAudioInputStream } from "./ByteBufferAudioInputStream";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInputStream } from "./WaveFileAudioInputStream";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

const FIRST_EVENT_ID: number = 1;
const IntermediateResultReceived: string = "IntermediateResultReceived";
const FinalResultReceived: string = "FinalResultReceived";
const Session: string = "Session";
const RecognitionErrorRaised: string = "RecognitionErrorRaised";

let eventIdentifier: number;

test("TranslationRecognizer1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizer("en-US", targets);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

test("TranslationRecognizer2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

test("GetSourceLanguage", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-us");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);
    expect(r.sourceLanguage).not.toBeUndefined();
    expect(r.sourceLanguage).not.toBeNull();
    expect(r.sourceLanguage).toEqual(language);

    r.close();
    s.close();
});

test("GetTargetLanguages", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-us");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);
    expect(r.targetLanguages).not.toBeUndefined();
    expect(r.targetLanguages).not.toBeNull();
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(language);

    r.close();
    s.close();
});

test.skip("GetOutputVoiceNameNoSetting", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const language: string = "en-US";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r.outputVoiceName).not.toBeUndefined();

    r.close();
    s.close();
});

test("GetOutputVoiceName", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const language: string = "en-US";
    const voice: string = "de-DE-Katja";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, language, targets, voice);

    expect(r.outputVoiceName).toEqual(voice);

    r.close();
    s.close();
});

test("GetParameters", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r.parameters).not.toBeUndefined();
    expect(r.sourceLanguage).toEqual(r.parameters.get(sdk.RecognizerParameterNames.TranslationFromLanguage, ""));

    // TODO this cannot be true, right? comparing an array with a string parameter???
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.parameters.get(sdk.RecognizerParameterNames.TranslationToLanguage + "0"));

    r.close();
    s.close();
});

test("RecognizeAsync1", (done: jest.DoneCallback) => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.failureReason);
        }
    });

    r.recognizeAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized).toEqual(res.translationStatus);
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");
    targets.push("en-US");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.failureReason);
        }
    });

    r.recognizeAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized).toEqual(res.translationStatus);
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

test("RecognizeAsync_badStream", (done: jest.DoneCallback) => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, language, targets);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.recognizeAsync(
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const eventsMap: { [id: string]: number; } = {};
    eventIdentifier = 1;

    r.FinalResultReceived = (o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
        eventsMap[FinalResultReceived] = eventIdentifier++;
    };

    r.IntermediateResultReceived = (o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[IntermediateResultReceived + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[IntermediateResultReceived] = now;
    };

    r.RecognitionErrorRaised = (o: sdk.Recognizer, e: sdk.RecognitionErrorEventArgs) => {
        eventsMap[RecognitionErrorRaised] = eventIdentifier++;
    };

    // TODO eventType should be renamed and be a function getEventType()
    r.RecognitionEvent = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[e.eventType.toString()] = now;
    };

    r.SessionEvent = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + ":" + e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[Session + ":" + e.eventType.toPrecision()] = now;
    };

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.failureReason);
        }
    });

    // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeAsync() call returns?!
    //      this is why below SessionStoppedEvent checks are conditional
    r.recognizeAsync((res: sdk.TranslationTextResult) => {

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
        expect(eventsMap[IntermediateResultReceived]).toBeLessThan(eventsMap[FinalResultReceived]);

        // make sure events we don't expect, don't get raised
        expect(RecognitionErrorRaised in eventsMap).toEqual(false);
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const rEvents: { [id: string]: string; } = {};

    r.FinalResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationTextResultEventArgs) => {
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const voice: string = "en-US-Zira";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, voice);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const rEvents: { [id: string]: Uint8Array; } = {};

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        const result: Uint8Array = e.result.audio;
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
            const result: Uint8Array = rEvents[Object.keys(rEvents)[0]];

            const inputStream = new ByteBufferAudioInputStream(result);

            const r2: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(inputStream, targets[0]);
            r2.recognizeAsync((speech: sdk.SpeechRecognitionResult) => {
                expect(speech.text).toEqual("What's the weather like?");
                r2.close();
                s.close();
                done();
            }, (error: string) => fail(error));
        }, (error: string) => fail(error));
    });
});

test("TranslateVoiceInvalidVoice", (done: jest.DoneCallback) => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-DE");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const voice: string = "de-DE-Hedda)";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, voice);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            expect(e.result.failureReason).toEqual("Synthesis service failed with code:  - Could not identify the voice 'de-DE-Hedda)' for the text to speech service ");
        } else {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail("Should have failed, instead got status='" + e.result.synthesisStatus.toString() + "'");
        }
    });

    r.RecognitionErrorRaised = ((o: sdk.Recognizer, e: sdk.RecognitionErrorEventArgs) => {
        r.close();
        s.close();
        setTimeout(() => done(), 1);
        fail(e);
    });

    r.startContinuousRecognitionAsync();
});

test("TranslateVoiceUSToGerman", (done: jest.DoneCallback) => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-DE");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const voice: string = "de-DE-Hedda";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, voice);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const rEvents: { [id: string]: Uint8Array; } = {};

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.failureReason);
        }

        const result: Uint8Array = e.result.audio;
        rEvents["Result@" + Date.now()] = result;
    });

    r.RecognitionErrorRaised = ((o: sdk.Recognizer, e: sdk.RecognitionErrorEventArgs) => {
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
            const result: Uint8Array = rEvents[Object.keys(rEvents)[0]];

            const inputStream = new ByteBufferAudioInputStream(result);

            const r2: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(inputStream, targets[0]);
            r2.recognizeAsync((speech: sdk.SpeechRecognitionResult) => {
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
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.LongFile);
    expect(ais).not.toBeUndefined();

    const voice: string = "en-US-Zira";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, voice);

    expect(r).not.toBeUndefined();

    expect(r instanceof sdk.Recognizer).toEqual(true);

    const rEvents: { [id: string]: Uint8Array; } = {};

    r.SynthesisResultReceived = ((o: sdk.Recognizer, e: sdk.TranslationSynthesisResultEventArgs) => {
        if (e.result.synthesisStatus === sdk.SynthesisStatus.Error) {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(e.result.failureReason);
        }

        const result: Uint8Array = e.result.audio;
        rEvents["Result@" + Date.now()] = result;
    });

    r.RecognitionErrorRaised = ((o: sdk.Recognizer, e: sdk.RecognitionErrorEventArgs) => {
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
                result.set(rEvents[value], byteCount);
                byteCount += rEvents[value].byteLength;
            });

            const inputStream = new ByteBufferAudioInputStream(result);

            const r2: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(inputStream, targets[0]);
            let constResult: string = "";
            let numEvents: number = 0;

            r2.FinalResultReceived = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
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
