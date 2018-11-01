//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    IAudioSource,
    IConnection,
    TranslationStatus,
} from "../common/Exports";
import {
    CancellationErrorCode,
    CancellationReason,
    PropertyCollection,
    ResultReason,
    TranslationRecognitionCanceledEventArgs,
    TranslationRecognitionEventArgs,
    TranslationRecognitionResult,
    TranslationRecognizer,
    Translations,
    TranslationSynthesisEventArgs,
    TranslationSynthesisResult,
} from "../sdk/Exports";
import {
    CancellationErrorCodePropertyName,
    EnumTranslation,
    RecognitionStatus,
    RequestSession,
    ServiceRecognizerBase,
    SynthesisStatus,
    TranslationHypothesis,
    TranslationPhrase,
    TranslationSynthesisEnd,
} from "./Exports";
import {
    IAuthentication,
} from "./IAuthentication";
import { IConnectionFactory } from "./IConnectionFactory";
import { RecognizerConfig } from "./RecognizerConfig";
import { SpeechConnectionMessage } from "./SpeechConnectionMessage.Internal";

// tslint:disable-next-line:max-classes-per-file
export class TranslationServiceRecognizer extends ServiceRecognizerBase {
    private translationRecognizer: TranslationRecognizer;

    public constructor(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioSource: IAudioSource,
        recognizerConfig: RecognizerConfig,
        translationRecognizer: TranslationRecognizer) {

        super(authentication, connectionFactory, audioSource, recognizerConfig, translationRecognizer);
        this.translationRecognizer = translationRecognizer;

    }

    protected ProcessTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: TranslationRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void {

        switch (connectionMessage.Path.toLowerCase()) {
            case "translation.hypothesis":

                const result: TranslationRecognitionEventArgs = this.FireEventForResult(TranslationHypothesis.FromJSON(connectionMessage.TextBody), requestSession);

                if (!!this.translationRecognizer.recognizing) {
                    try {
                        this.translationRecognizer.recognizing(this.translationRecognizer, result);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                }

                break;
            case "translation.phrase":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }

                const translatedPhrase: TranslationPhrase = TranslationPhrase.FromJSON(connectionMessage.TextBody);

                if (translatedPhrase.RecognitionStatus === RecognitionStatus.Success) {
                    // OK, the recognition was successful. How'd the translation do?
                    const result: TranslationRecognitionEventArgs = this.FireEventForResult(translatedPhrase, requestSession);
                    if (!!this.translationRecognizer.recognized) {
                        try {
                            this.translationRecognizer.recognized(this.translationRecognizer, result);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }

                    // report result to promise.
                    if (!!successCallback) {
                        try {
                            successCallback(result.result);
                        } catch (e) {
                            if (!!errorCallBack) {
                                errorCallBack(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        successCallback = undefined;
                        errorCallBack = undefined;
                    }

                    break;
                } else {
                    const reason: ResultReason = EnumTranslation.implTranslateRecognitionResult(translatedPhrase.RecognitionStatus);

                    const result = new TranslationRecognitionResult(
                        undefined,
                        requestSession.RequestId,
                        reason,
                        translatedPhrase.Text,
                        translatedPhrase.Duration,
                        translatedPhrase.Offset,
                        undefined,
                        connectionMessage.TextBody,
                        undefined);

                    if (reason === ResultReason.Canceled) {
                        const cancelReason: CancellationReason = EnumTranslation.implTranslateCancelResult(translatedPhrase.RecognitionStatus);

                        const ev = new TranslationRecognitionCanceledEventArgs(
                            requestSession.SessionId,
                            cancelReason,
                            null,
                            cancelReason === CancellationReason.Error ? CancellationErrorCode.ServiceError : CancellationErrorCode.NoError,
                            result);

                        if (!!this.translationRecognizer.canceled) {
                            try {
                                this.translationRecognizer.canceled(this.translationRecognizer, ev);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    } else {
                        const ev = new TranslationRecognitionEventArgs(result, 0/*offset*/, requestSession.SessionId);

                        if (!!this.translationRecognizer.recognized) {
                            try {
                                this.translationRecognizer.recognized(this.translationRecognizer, ev);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    }

                    // report result to promise.
                    if (!!successCallback) {
                        try {
                            successCallback(result);
                        } catch (e) {
                            if (!!errorCallBack) {
                                errorCallBack(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        successCallback = undefined;
                        errorCallBack = undefined;
                    }
                }
                break;

            case "translation.synthesis":
                this.sendSynthesisAudio(connectionMessage.BinaryBody, requestSession.SessionId);
                break;

            case "translation.synthesis.end":
                const synthEnd: TranslationSynthesisEnd = TranslationSynthesisEnd.FromJSON(connectionMessage.TextBody);

                switch (synthEnd.SynthesisStatus) {
                    case SynthesisStatus.Error:
                        if (!!this.translationRecognizer.synthesizing) {
                            const result = new TranslationSynthesisResult(ResultReason.Canceled, undefined);
                            const retEvent: TranslationSynthesisEventArgs = new TranslationSynthesisEventArgs(result, requestSession.SessionId);

                            try {
                                this.translationRecognizer.synthesizing(this.translationRecognizer, retEvent);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }

                        if (!!this.translationRecognizer.canceled) {
                            // And raise a canceled event to send the rich(er) error message back.
                            const canceledResult: TranslationRecognitionCanceledEventArgs = new TranslationRecognitionCanceledEventArgs(
                                requestSession.SessionId,
                                CancellationReason.Error,
                                synthEnd.FailureReason,
                                CancellationErrorCode.ServiceError,
                                null);

                            try {
                                this.translationRecognizer.canceled(this.translationRecognizer, canceledResult);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                        break;
                    case SynthesisStatus.Success:
                        this.sendSynthesisAudio(undefined, requestSession.SessionId);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    protected ConnectionError(sessionId: string, requestId: string, error: string): void {
        if (!!this.translationRecognizer.canceled) {
            const properties: PropertyCollection = new PropertyCollection();
            properties.setProperty(CancellationErrorCodePropertyName, CancellationErrorCode[CancellationErrorCode.ConnectionFailure]);

            const result: TranslationRecognitionResult = new TranslationRecognitionResult(
                undefined,
                requestId,
                ResultReason.Canceled,
                undefined,
                undefined,
                undefined,
                error,
                undefined,
                properties);

            const cancelEvent: TranslationRecognitionCanceledEventArgs = new TranslationRecognitionCanceledEventArgs(
                sessionId,
                CancellationReason.Error,
                error,
                CancellationErrorCode.ConnectionFailure,
                result,
            );
            try {
                this.translationRecognizer.canceled(this.translationRecognizer, cancelEvent);
                /* tslint:disable:no-empty */
            } catch { }
        }

    }

    private FireEventForResult(serviceResult: TranslationHypothesis | TranslationPhrase, requestSession: RequestSession): TranslationRecognitionEventArgs {
        let translations: Translations;

        if (undefined !== serviceResult.Translation.Translations) {
            translations = new Translations();
            for (const translation of serviceResult.Translation.Translations) {
                translations.set(translation.Language, translation.Text);
            }
        }

        let resultReason: ResultReason;
        if (serviceResult instanceof TranslationPhrase) {
            if (serviceResult.Translation.TranslationStatus === TranslationStatus.Success) {
                resultReason = ResultReason.TranslatedSpeech;
            } else {
                resultReason = ResultReason.RecognizedSpeech;
            }
        } else {
            resultReason = ResultReason.TranslatingSpeech;
        }

        const result = new TranslationRecognitionResult(
            translations,
            requestSession.RequestId,
            resultReason,
            serviceResult.Text,
            serviceResult.Duration,
            serviceResult.Offset,
            serviceResult.Translation.FailureReason,
            JSON.stringify(serviceResult),
            undefined);

        const ev = new TranslationRecognitionEventArgs(result, serviceResult.Offset, requestSession.SessionId);
        return ev;
    }

    private sendSynthesisAudio(audio: ArrayBuffer, sessionId: string): void {
        const reason = (undefined === audio) ? ResultReason.SynthesizingAudioCompleted : ResultReason.SynthesizingAudio;
        const result = new TranslationSynthesisResult(reason, audio);
        const retEvent: TranslationSynthesisEventArgs = new TranslationSynthesisEventArgs(result, sessionId);

        if (!!this.translationRecognizer.synthesizing) {
            try {
                this.translationRecognizer.synthesizing(this.translationRecognizer, retEvent);
                /* tslint:disable:no-empty */
            } catch (error) {
                // Not going to let errors in the event handler
                // trip things up.
            }
        }

    }
}
