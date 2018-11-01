//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import {
    IAudioSource,
    IConnection,
} from "../common/Exports";
import {
    CancellationErrorCode,
    CancellationReason,
    OutputFormat,
    PropertyCollection,
    ResultReason,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionEventArgs,
    SpeechRecognitionResult,
    SpeechRecognizer,
} from "../sdk/Exports";
import {
    CancellationErrorCodePropertyName,
    DetailedSpeechPhrase,
    EnumTranslation,
    OutputFormatPropertyName,
    RecognitionStatus,
    RequestSession,
    ServiceRecognizerBase,
    SimpleSpeechPhrase,
    SpeechHypothesis,
} from "./Exports";
import {
    IAuthentication,
} from "./IAuthentication";
import { IConnectionFactory } from "./IConnectionFactory";
import { RecognizerConfig } from "./RecognizerConfig";
import { SpeechConnectionMessage } from "./SpeechConnectionMessage.Internal";

// tslint:disable-next-line:max-classes-per-file
export class SpeechServiceRecognizer extends ServiceRecognizerBase {

    private speechRecognizer: SpeechRecognizer;

    public constructor(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioSource: IAudioSource,
        recognizerConfig: RecognizerConfig,
        speechRecognizer: SpeechRecognizer) {
        super(authentication, connectionFactory, audioSource, recognizerConfig, speechRecognizer);
        this.speechRecognizer = speechRecognizer;
    }

    protected ProcessTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: SpeechRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void {

        let result: SpeechRecognitionResult;

        switch (connectionMessage.Path.toLowerCase()) {
            case "speech.hypothesis":
                const hypothesis: SpeechHypothesis = SpeechHypothesis.FromJSON(connectionMessage.TextBody);

                result = new SpeechRecognitionResult(
                    requestSession.RequestId,
                    ResultReason.RecognizingSpeech,
                    hypothesis.Text,
                    hypothesis.Duration,
                    hypothesis.Offset,
                    undefined,
                    connectionMessage.TextBody,
                    undefined);

                const ev = new SpeechRecognitionEventArgs(result, hypothesis.Duration, requestSession.SessionId);

                if (!!this.speechRecognizer.recognizing) {
                    try {
                        this.speechRecognizer.recognizing(this.speechRecognizer, ev);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                }
                break;
            case "speech.phrase":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }

                const simple: SimpleSpeechPhrase = SimpleSpeechPhrase.FromJSON(connectionMessage.TextBody);
                const resultReason: ResultReason = EnumTranslation.implTranslateRecognitionResult(simple.RecognitionStatus);

                if (ResultReason.Canceled === resultReason) {
                    result = new SpeechRecognitionResult(
                        requestSession.RequestId,
                        resultReason,
                        undefined,
                        undefined,
                        undefined,
                        undefined,
                        connectionMessage.TextBody,
                        undefined);

                    if (!!this.speechRecognizer.canceled) {
                        const cancelEvent: SpeechRecognitionCanceledEventArgs = new SpeechRecognitionCanceledEventArgs(
                            EnumTranslation.implTranslateCancelResult(simple.RecognitionStatus),
                            undefined,
                            undefined,
                            EnumTranslation.implTranslateCancelResult(simple.RecognitionStatus) === CancellationReason.Error ? CancellationErrorCode.ServiceError : CancellationErrorCode.NoError,
                            requestSession.SessionId);
                        try {
                            this.speechRecognizer.canceled(this.speechRecognizer, cancelEvent);
                            /* tslint:disable:no-empty */
                        } catch { }
                    }
                } else {
                    if (this.recognizerConfig.parameters.getProperty(OutputFormatPropertyName) === OutputFormat[OutputFormat.Simple]) {
                        result = new SpeechRecognitionResult(
                            requestSession.RequestId,
                            EnumTranslation.implTranslateRecognitionResult(simple.RecognitionStatus),
                            simple.DisplayText,
                            simple.Duration,
                            simple.Offset,
                            undefined,
                            connectionMessage.TextBody,
                            undefined);
                    } else {
                        const detailed: DetailedSpeechPhrase = DetailedSpeechPhrase.FromJSON(connectionMessage.TextBody);

                        result = new SpeechRecognitionResult(
                            requestSession.RequestId,
                            EnumTranslation.implTranslateRecognitionResult(detailed.RecognitionStatus),
                            detailed.RecognitionStatus === RecognitionStatus.Success ? detailed.NBest[0].Display : undefined,
                            detailed.Duration,
                            detailed.Offset,
                            undefined,
                            connectionMessage.TextBody,
                            undefined);
                    }

                    const event: SpeechRecognitionEventArgs = new SpeechRecognitionEventArgs(result, result.offset, requestSession.SessionId);

                    if (!!this.speechRecognizer.recognized) {
                        try {
                            this.speechRecognizer.recognized(this.speechRecognizer, event);
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

                break;
            default:
                break;
        }
    }

    protected ConnectionError(sessionId: string, requestId: string, error: string): void {
        if (!!this.speechRecognizer.canceled) {
            const properties: PropertyCollection = new PropertyCollection();
            properties.setProperty(CancellationErrorCodePropertyName, CancellationErrorCode[CancellationErrorCode.ConnectionFailure]);

            const cancelEvent: SpeechRecognitionCanceledEventArgs = new SpeechRecognitionCanceledEventArgs(
                CancellationReason.Error,
                error,
                CancellationErrorCode.ConnectionFailure,
                undefined,
                sessionId);
            try {
                this.speechRecognizer.canceled(this.speechRecognizer, cancelEvent);
                /* tslint:disable:no-empty */
            } catch { }
        }

    }
}
