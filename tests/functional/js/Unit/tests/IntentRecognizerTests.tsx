//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { setTimeout } from "timers";
import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { ByteBufferAudioFile } from "./ByteBufferAudioFile";
import { Settings } from "./Settings";
import { default as WaitForCondition } from "./Utilities";
import { WaveFileAudioInput } from "./WaveFileAudioInputStream";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

const ValidateResultMatchesWaveFile = (res: sdk.SpeechRecognitionResult): void => {
    expect(res).not.toBeUndefined();
    expect(res.text).toEqual(Settings.LuisWavFileText);
    expect(res.duration).toBeCloseTo(Settings.LuisWaveFileDuration);
    expect(res.offset).toBeCloseTo(Settings.LuisWaveFileOffset);
};

test("NoIntentsRecognizesSpeech", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(res).not.toBeUndefined();
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("AddNullIntent", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    expect(() => r.addIntent("phrase", null)).toThrow();

    r.close();
    s.close();
});

test("AddNullPhrase", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    expect(() => r.addIntent(null, "ID")).toThrow();

    r.close();
    s.close();
});

test("RoundTripWithGoodIntent", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

class BadLangModel extends sdk.LanguageUnderstandingModel {
    public constructor() {
        super();
    }
    public appId: string;
}

test("AddIntentWithBadModel", () => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();
    s.speechRecognitionLanguage = "en-US";

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const langModel: BadLangModel = new BadLangModel();
    langModel.appId = "";

    expect(() => r.addIntentWithLanguageModel("IntentId", langModel, "IntentName")).toThrow();
});

test("InitialSilenceTimeout", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
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

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);
    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    let oneReport: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        setTimeout(() => done(), 1);
        fail(e.errorDetails);
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.SpeechRecognitionEventArgs) => {
        const res: sdk.SpeechRecognitionResult = e.result;
        expect(res).not.toBeUndefined();
        expect(res.reason).toEqual(sdk.ResultReason.NoMatch);
        expect(res.text).toBeUndefined();

        const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
        expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

        if (true === oneReport) {
            done();
        }

        oneReport = true;
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            const res: sdk.SpeechRecognitionResult = p2;

            expect(res).not.toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.NoMatch);
            expect(res.text).toBeUndefined();

            const nmd: sdk.NoMatchDetails = sdk.NoMatchDetails.fromResult(res);
            expect(nmd.reason).toEqual(sdk.NoMatchReason.InitialSilenceTimeout);

            r.close();
            s.close();
            if (true === oneReport) {
                done();
            }

            oneReport = true;
        },
        (error: string) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            fail(error);
        });
}, 7500);

// The service never responds beyond turn.start...
test.skip("emptyFile", (done: jest.DoneCallback) => {

    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = "en-US";

    const blob: Blob[] = [];
    const f: File = new File(blob, "file.wav");

    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    let oneCalled: boolean = false;

    r.canceled = (o: sdk.Recognizer, e: sdk.IntentRecognitionCanceledEventArgs): void => {
        expect(e.reason).toEqual(sdk.CancellationReason.Error);
        const cancelDetails: sdk.CancellationDetails = sdk.CancellationDetails.fromResult(e.result);
        expect(cancelDetails.reason).toEqual(sdk.CancellationReason.Error);

        if (true === oneCalled) {
            done();
        } else {
            oneCalled = true;
        }
    };

    r.recognizeOnceAsync(
        (p2: sdk.SpeechRecognitionResult) => {
            setTimeout(() => done(), 1);
            r.close();
            s.close();
            fail("Error handler wasn't called");
        },
        (error: string) => {
            if (true === oneCalled) {
                done();
            } else {
                oneCalled = true;
            }
        });
});

test("Continous Recog With Intent", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromSubscription(Settings.LuisAppKey, Settings.LuisAppId, Settings.LuisRegion);
    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        setTimeout(() => done(), 1);
        fail(e.errorDetails);
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs) => {
        const res: sdk.IntentRecognitionResult = e.result;
        expect(res).not.toBeUndefined();
        expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
        expect(res.intentId).toEqual(Settings.LuisValidIntentId);
        ValidateResultMatchesWaveFile(res);

        r.stopContinuousRecognitionAsync(() => {
            r.close();
            s.close();
            done();
        }, (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
    };

    r.startContinuousRecognitionAsync(
        /* tslint:disable:no-empty */
        () => { },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("RoundTripWithGoodModelWrongIntent", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId + "-Bad", lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedSpeech);
            expect(res.intentId).toBeUndefined();
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test.skip("MultiPhrase Intent", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
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
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromSubscription(Settings.LuisAppKey, Settings.LuisAppId, Settings.LuisRegion);
    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm);

    let numIntents: number = 0;

    r.canceled = (o: sdk.Recognizer, e: sdk.SpeechRecognitionCanceledEventArgs) => {
        switch (e.reason) {
            case sdk.CancellationReason.Error:
                setTimeout(() => done(), 1);
                fail(e.errorDetails);
                break;
            case sdk.CancellationReason.EndOfStream:
                expect(numIntents).toEqual(numPhrases);
                r.stopContinuousRecognitionAsync(() => {
                    r.close();
                    s.close();
                    done();
                });
                break;
        }
    };

    r.recognized = (o: sdk.Recognizer, e: sdk.IntentRecognitionEventArgs) => {
        const res: sdk.IntentRecognitionResult = e.result;
        expect(res).not.toBeUndefined();
        expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
        expect(res.intentId).toEqual(Settings.LuisValidIntentId);
        ValidateResultMatchesWaveFile(res);
        numIntents++;

    };

    r.startContinuousRecognitionAsync(
        /* tslint:disable:no-empty */
        () => { },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
}, 15000);

test("IntentAlias", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);
    const intentName: string = "SomeName";

    r.addIntentWithLanguageModel(Settings.LuisValidIntentId, lm, intentName);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(res.reason).toEqual(sdk.ResultReason.RecognizedIntent);
            expect(res.intentId).toEqual(intentName);
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("Add All Intents", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addAllIntents(lm);

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual(Settings.LuisValidIntentId);
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});

test("Add All Intents with alias", (done: jest.DoneCallback) => {
    const s: sdk.SpeechConfig = sdk.SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
    expect(s).not.toBeUndefined();

    s.speechRecognitionLanguage = Settings.LuisWaveFileLanguage;

    const f: File = WaveFileAudioInput.LoadFile(Settings.LuisWaveFile);
    const config: sdk.AudioConfig = sdk.AudioConfig.fromWavFileInput(f);

    const r: sdk.IntentRecognizer = new sdk.IntentRecognizer(s, config);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);

    const lm: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);

    r.addAllIntents(lm, "alias");

    r.recognizeOnceAsync(
        (p2: sdk.IntentRecognitionResult) => {
            r.close();
            s.close();
            setTimeout(() => done(), 1);
            const res: sdk.IntentRecognitionResult = p2;
            expect(res).not.toBeUndefined();
            expect(res.errorDetails).toBeUndefined();
            expect(sdk.ResultReason[res.reason]).toEqual(sdk.ResultReason[sdk.ResultReason.RecognizedIntent]);
            expect(res.intentId).toEqual("alias");
            ValidateResultMatchesWaveFile(res);
        },
        (error: string) => {
            setTimeout(() => done(), 1);
            fail(error);
        });
});
