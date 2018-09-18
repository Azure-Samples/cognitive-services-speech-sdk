//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { setTimeout } from "timers";
import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { RecognitionEventType, SessionEventType } from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { Settings } from "./Settings";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

const FIRST_EVENT_ID: number = 1;
const Recognizing: string = "Recognizing";
const Recognized: string = "Recognized";
const Session: string = "Session";
const Canceled: string = "Canceled";

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

    r.recognizeOnceAsync(
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

test("testGetOutputFormatDetailed", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const f: File = WaveFileAudioInput.LoadFile(Settings.WaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = "de-DE";
    s.speechRecognitionLanguage = language;
    s.outputFormat = sdk.OutputFormat.Detailed;

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
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("testRecognizeOnceAsync2", (done: jest.DoneCallback) => {
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

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
        eventsMap[Recognized] = eventIdentifier++;
    };

    r.recognizing = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
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
        eventsMap[RecognitionEventType.SpeechStartDetectedEvent.toString()] = now;
    };
    r.speechEndDetected = (o: sdk.Recognizer, e: sdk.RecognitionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[RecognitionEventType.SpeechEndDetectedEvent.toString()] = now;
    };

    r.sessionStarted = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + SessionEventType.SessionStartedEvent.toString()] = now;
        eventsMap[Session + SessionEventType.SessionStartedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
    };
    r.sessionStopped = (o: sdk.Recognizer, e: sdk.SessionEventArgs) => {
        const now: number = eventIdentifier++;
        eventsMap[Session + SessionEventType.SessionStoppedEvent.toString()] = now;
        eventsMap[Session + SessionEventType.SessionStoppedEvent.toString() + "-" + Date.now().toPrecision(4)] = now;
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

            expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[Recognized]);

            // recognition events come after session start but before session end events
            expect(eventsMap[Session + sdk.SessionEventType.SessionStartedEvent.toString()])
                .toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

            if (Session + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
                expect(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()])
                    .toBeLessThan(eventsMap[Session + sdk.SessionEventType.SessionStoppedEvent.toString()]);
            }

            // there is no partial result reported after the final result
            // (and check that we have intermediate and final results recorded)
            if (Recognizing in eventsMap) {
                expect(eventsMap[Recognizing])
                    .toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);
            }

            // speech should stop before getting the final result.
            expect(eventsMap[Recognized]).toBeGreaterThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

            expect(eventsMap[Recognizing]).toBeLessThan(eventsMap[Recognized]);

            // make sure events we don't expect, don't get raised
            expect(Canceled in eventsMap).toBeFalsy();

            r.close();
            s.close();
            done();
        }, (error: string) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(error);
        });

}, 30000);

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
            setTimeout(() => done(), 1);
            fail(err);
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
                    setTimeout(() => done(), 1);
                    fail(err);
                });
        },
        (err: string) => {
            setTimeout(() => done(), 1);
            fail(err);
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

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionResultEventArgs) => {
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
            setTimeout(() => done(), 1);
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
    // ??     assertNotNull(r.getRecoImpl());
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
            setTimeout(() => done(), 1);
            fail(error);
        });
}, 10000);

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
            setTimeout(() => done(), 1);
            fail(error);
        });

    const sendSize: number = 4096;

    for (i = sendSize - 1; i < f.byteLength; i += sendSize) {
        p.write(f.slice(i - (sendSize - 1), i));
    }

    p.write(f.slice(i - (sendSize - 1), f.byteLength - 1));
    p.close();

}, 10000);

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
            setTimeout(() => done(), 1);
            fail(error);
        });
}, 10000);

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
            setTimeout(() => done(), 1);
            fail(error);
        });
}, 10000);
