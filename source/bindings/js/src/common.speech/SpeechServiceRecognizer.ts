// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { IAudioSource, IConnection } from "../common/Exports";
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
import { IAuthentication } from "./IAuthentication";
import { IConnectionFactory } from "./IConnectionFactory";
import { RecognizerConfig } from "./RecognizerConfig";
import { SpeechConnectionMessage } from "./SpeechConnectionMessage.Internal";

// tslint:disable-next-line:max-classes-per-file
export class SpeechServiceRecognizer extends ServiceRecognizerBase {

    private privSpeechRecognizer: SpeechRecognizer;

    public constructor(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioSource: IAudioSource,
        recognizerConfig: RecognizerConfig,
        speechRecognizer: SpeechRecognizer) {
        super(authentication, connectionFactory, audioSource, recognizerConfig, speechRecognizer);
        this.privSpeechRecognizer = speechRecognizer;
    }

    protected processTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: SpeechRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void {

        let result: SpeechRecognitionResult;

        switch (connectionMessage.path.toLowerCase()) {
            case "speech.hypothesis":
                const hypothesis: SpeechHypothesis = SpeechHypothesis.fromJSON(connectionMessage.textBody);

                result = new SpeechRecognitionResult(
                    requestSession.requestId,
                    ResultReason.RecognizingSpeech,
                    hypothesis.Text,
                    hypothesis.Duration,
                    hypothesis.Offset,
                    undefined,
                    connectionMessage.textBody,
                    undefined);

                const ev = new SpeechRecognitionEventArgs(result, hypothesis.Duration, requestSession.sessionId);

                if (!!this.privSpeechRecognizer.recognizing) {
                    try {
                        this.privSpeechRecognizer.recognizing(this.privSpeechRecognizer, ev);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                }
                break;
            case "speech.phrase":
                if (this.privRecognizerConfig.isContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.sendTelemetryData(requestSession.requestId, connection, requestSession.getTelemetry());
                }

                const simple: SimpleSpeechPhrase = SimpleSpeechPhrase.fromJSON(connectionMessage.textBody);
                const resultReason: ResultReason = EnumTranslation.implTranslateRecognitionResult(simple.RecognitionStatus);

                if (ResultReason.Canceled === resultReason) {
                    result = new SpeechRecognitionResult(
                        requestSession.requestId,
                        resultReason,
                        undefined,
                        undefined,
                        undefined,
                        undefined,
                        connectionMessage.textBody,
                        undefined);

                    if (!!this.privSpeechRecognizer.canceled) {
                        const cancelEvent: SpeechRecognitionCanceledEventArgs = new SpeechRecognitionCanceledEventArgs(
                            EnumTranslation.implTranslateCancelResult(simple.RecognitionStatus),
                            undefined,
                            undefined,
                            EnumTranslation.implTranslateCancelResult(simple.RecognitionStatus) === CancellationReason.Error ? CancellationErrorCode.ServiceError : CancellationErrorCode.NoError,
                            requestSession.sessionId);
                        try {
                            this.privSpeechRecognizer.canceled(this.privSpeechRecognizer, cancelEvent);
                            /* tslint:disable:no-empty */
                        } catch { }
                    }
                } else {
                    if (this.privRecognizerConfig.parameters.getProperty(OutputFormatPropertyName) === OutputFormat[OutputFormat.Simple]) {
                        result = new SpeechRecognitionResult(
                            requestSession.requestId,
                            EnumTranslation.implTranslateRecognitionResult(simple.RecognitionStatus),
                            simple.DisplayText,
                            simple.Duration,
                            simple.Offset,
                            undefined,
                            connectionMessage.textBody,
                            undefined);
                    } else {
                        const detailed: DetailedSpeechPhrase = DetailedSpeechPhrase.fromJSON(connectionMessage.textBody);

                        result = new SpeechRecognitionResult(
                            requestSession.requestId,
                            EnumTranslation.implTranslateRecognitionResult(detailed.RecognitionStatus),
                            detailed.RecognitionStatus === RecognitionStatus.Success ? detailed.NBest[0].Display : undefined,
                            detailed.Duration,
                            detailed.Offset,
                            undefined,
                            connectionMessage.textBody,
                            undefined);
                    }

                    const event: SpeechRecognitionEventArgs = new SpeechRecognitionEventArgs(result, result.offset, requestSession.sessionId);

                    if (!!this.privSpeechRecognizer.recognized) {
                        try {
                            this.privSpeechRecognizer.recognized(this.privSpeechRecognizer, event);
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

    protected connectionError(sessionId: string, requestId: string, error: string): void {
        if (!!this.privSpeechRecognizer.canceled) {
            const properties: PropertyCollection = new PropertyCollection();
            properties.setProperty(CancellationErrorCodePropertyName, CancellationErrorCode[CancellationErrorCode.ConnectionFailure]);

            const cancelEvent: SpeechRecognitionCanceledEventArgs = new SpeechRecognitionCanceledEventArgs(
                CancellationReason.Error,
                error,
                CancellationErrorCode.ConnectionFailure,
                undefined,
                sessionId);
            try {
                this.privSpeechRecognizer.canceled(this.privSpeechRecognizer, cancelEvent);
                /* tslint:disable:no-empty */
            } catch { }
        }

    }
}
