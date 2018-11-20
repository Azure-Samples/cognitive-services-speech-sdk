// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { setTimeout } from "timers";

import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { ConsoleLoggingListener } from "../../../../../source/bindings/js/src/common.browser/Exports";
import { Events, EventType } from "../../../../../source/bindings/js/src/common/Exports";

import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";
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

const ValidateResultMatchesWaveFile = (res: sdk.SpeechRecognitionResult): void => {
    expect(res).not.toBeUndefined();
    expect(res.text).toEqual(Settings.LuisWavFileText);
    expect(Math.abs(res.duration - Settings.LuisWaveFileDuration) / Settings.LuisWaveFileDuration).toBeLessThanOrEqual(0.05);
    expect(Math.abs(res.offset - Settings.LuisWaveFileOffset) / Settings.LuisWaveFileOffset).toBeLessThanOrEqual(0.05);
};

const BuildRecognizerFromWaveFile: (speechConfig?: sdk.SpeechConfig) => sdk.IntentRecognizer = (speechConfig?: sdk.SpeechConfig): sdk.IntentRecognizer => {

    let s: sdk.SpeechConfig = speechConfig;
    if (s === undefined) {
        s = BuildSpeechConfig();
        // Since we're not going to return it, mark it for closure.
        objsToClose.push(s);
    }

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const language: string = Settings.WaveFileLanguage;
    if (s.speechRecognitionLanguage === undefined) {
        s.speechRecognitionLanguage = language;
    }

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();

    return r;
};

const BuildSpeechConfig: () => sdk.SpeechConfig = (): sdk.SpeechConfig => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();
    return s;
};

test("NoIntentsRecognizesSpeech", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {

            const res: sdk.IntentRecognitionResult = p2;
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(res).not.toBeUndefined();
            ValidateResultMatchesWaveFile(res);
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("AddNullIntent", () => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(() => r.addIntent("phrase", null)).toThrow();
});

test("AddNullPhrase", () => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    expect(() => r.addIntent(null, "ID")).toThrow();
});

test("RoundTripWithGoodIntent", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

class BadLangModel extends sdk.LanguageUnderstandingModel {
    public constructor() {
        super();
    }
    public appId: string;
}

test("AddIntentWithBadModel", () => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const langModel: BadLangModel = new BadLangModel();
    langModel.appId = "";

    expect(() => r.addIntentWithLanguageModel("IntentId", langModel, "IntentName")).toThrow();
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

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    let numReports: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
        done.fail(e.errorDetails);
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs) => {
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
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            numReports++;

            expect(res).not.toBeUndefined();
            expect(sdk.ResultReason.NoMatch).toEqual(res.reason);
            expect(res.errorDetails).toBeUndefined();
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);
            expect(Date.now()).toBeGreaterThan(startTime + 2000);

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

test("Continous Recog With Intent", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromSubscription(Settings.LuisAppKey, Settings.LuisAppId, Settings.LuisRegion);
    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
        done.fail(e.errorDetails);
    };

    r.recognizing = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs): void => {
        try {
            expect(e.result.reason).toEqual(sdk.ResultReason.RecognizingIntent);
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs) => {
        try {
            const res: sdk.IntentRecognitionResult = e.result;
            expect(res).not.toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);

            r.stopContinuousRecognitionAsync(() => {
                done();
            }, (error: string) => {
                done.fail(error);
            });
        } catch (error) {
            done.fail(error);
        }
    };

    r.startContinuousRecognitionAsync(
        /* tslint:disable:no-empty */
        () => { },
        (error: string) => {
            setTimeout(done, 1);
            fail(error);
        });
});

test("RoundTripWithGoodModelWrongIntent", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId + "-Bad", lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(res.intentId).toBeUndefined();
            ValidateResultMatchesWaveFile(res);
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test.skip("MultiPhrase Intent", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);

    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: ArrayBuffer = WaveFileAudioInput.LoadArrayFromFile(Settings.LuisWaveFile);
    const p: sdk.PushAudioInputStream = sdk.AudioInputStream.createPushStream();
    const config: sdk.AudioConfig = sdk.AudioConfig.fromStreamInput(p);
    const numPhrases: number = 3;

    // Adding some extra silence to ensure SR goes smmothly since the goal here isn't to test
    // the SR engine, but rather the multi-phrase reconnect code.
    const silenceBuffer: Uint8Array = new Uint8Array(16 * 1024); // ~500ms

    for (let i: number = 0; i < 2; i++) {
        p.write(f);
        p.write(silenceBuffer.buffer);
    }

    p.close();

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    objsToClose.push(r);

    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromSubscription(Settings.LuisAppKey, Settings.LuisAppId, Settings.LuisRegion);
    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    let numIntents: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
        try {
            switch (e.reason) {
                case sdk.CancellationReason.Error:
                    done.fail(e.errorDetails);
                    break;
                case sdk.CancellationReason.EndOfStream:
                    expect(numIntents).toEqual(numPhrases);
                    r.stopContinuousRecognitionAsync(() => {
                        done();
                    });
                    break;
            }
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs) => {
        try {
            const res: sdk.IntentRecognitionResult = e.result;
            expect(res).not.toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);
            numIntents++;
        } catch (error) {
            done.fail(error);
        }
    };

    r.startContinuousRecognitionAsync(
        /* tslint:disable:no-empty */
        () => { },
        (error: string) => {
            done.fail(error);
        });
}, 15000);

test("IntentAlias", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);
    const intentName: string = "SomeName";

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm, intentName);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
            expect(res.intentId).toEqual(intentName);
            ValidateResultMatchesWaveFile(res);
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("Add All Intents", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addAllIntents(lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);
            expect(res.properties.getProperty(sdk.PropertyId.LanguageUnderstandingServiceResponse_JsonResult)).not.toBeUndefined();
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("Add All Intents with alias", (done: jest.DoneCallback) => {
    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile();
    objsToClose.push(r);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addAllIntents(lm, "alias");

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual("alias");
            ValidateResultMatchesWaveFile(res);
            done();
        },
        (error: string) => {
            done.fail(error);
        });
});

test("Audio Config is optional", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    objsToClose.push(s);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s);
    objsToClose.push(r);
    expect(r instanceof sdk.Recognizer).toEqual(true);
});

test("Default mic is used when audio config is not specified.", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s);
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
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription("badKey", Settings.SpeechRegion);
    objsToClose.push(s);

    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
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

    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);

    let doneCount: number = 0;
    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
        try {
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
            expect(e.errorDetails).toContain("1006");
            doneCount++;
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync((result: sdk.IntentRecognitionResult) => {
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

// Truman does not behave the same as SkyMan for a bad language. It closes the connection far more gracefully.
test.skip("RecognizeOnce Bad Language", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = BuildSpeechConfig();
    objsToClose.push(s);
    s.speechRecognitionLanguage = "BadLanguage";

    const r: sdk.IntentRecognizer = BuildRecognizerFromWaveFile(s);
    objsToClose.push(r);
    let doneCount: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs) => {
        try {
            expect(sdk.CancellationReason[e.reason]).toEqual(sdk.CancellationReason[sdk.CancellationReason.Error]);
            expect(sdk.CancellationErrorCode[e.errorCode]).toEqual(sdk.CancellationErrorCode[sdk.CancellationErrorCode.ConnectionFailure]);
            expect(e.errorDetails).toContain("1007");
            doneCount++;
        } catch (error) {
            done.fail(error);
        }
    };

    r.recognizeOnceAsync((result: sdk.IntentRecognitionResult) => {
        done.fail("RecognizeOnceAsync did not fail");
    }, (error: string) => {
        try {
            expect(error).toContain("1007");
        } catch (error) {
            done.fail(error);
        }
        doneCount++;
    });

    WaitForCondition(() => (doneCount === 2), done);
});
