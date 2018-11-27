// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { IAudioSource, IConnection } from "../common/Exports";
import {
    CancellationErrorCode,
    CancellationReason,
    IntentRecognitionCanceledEventArgs,
    IntentRecognitionEventArgs,
    IntentRecognitionResult,
    IntentRecognizer,
    PropertyCollection,
    PropertyId,
    ResultReason,
} from "../sdk/Exports";
import {
    AddedLmIntent,
    CancellationErrorCodePropertyName,
    EnumTranslation,
    IntentResponse,
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
export class IntentServiceRecognizer extends ServiceRecognizerBase {
    private privIntentRecognizer: IntentRecognizer;
    private privAddedLmIntents: { [id: string]: AddedLmIntent; };
    private privIntentDataSent: boolean;
    private privUmbrellaIntent: AddedLmIntent;
    private privPendingIntentArgs: IntentRecognitionEventArgs;

    public constructor(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioSource: IAudioSource,
        recognizerConfig: RecognizerConfig,
        recognizer: IntentRecognizer,
        intentDataSent: boolean) {
        super(authentication, connectionFactory, audioSource, recognizerConfig, recognizer);
        this.privIntentRecognizer = recognizer;
        this.privIntentDataSent = intentDataSent;
    }

    public setIntents(addedIntents: { [id: string]: AddedLmIntent; }, umbrellaIntent: AddedLmIntent): void {
        this.privAddedLmIntents = addedIntents;
        this.privUmbrellaIntent = umbrellaIntent;
    }

    protected processTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: IntentRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void {

        let result: IntentRecognitionResult;
        let ev: IntentRecognitionEventArgs;

        switch (connectionMessage.path.toLowerCase()) {
            case "speech.hypothesis":
                const speechHypothesis: SpeechHypothesis = SpeechHypothesis.fromJSON(connectionMessage.textBody);

                result = new IntentRecognitionResult(
                    undefined,
                    requestSession.requestId,
                    ResultReason.RecognizingIntent,
                    speechHypothesis.Text,
                    speechHypothesis.Duration,
                    speechHypothesis.Offset,
                    undefined,
                    connectionMessage.textBody,
                    undefined);

                ev = new IntentRecognitionEventArgs(result, speechHypothesis.Offset, requestSession.sessionId);

                if (!!this.privIntentRecognizer.recognizing) {
                    try {
                        this.privIntentRecognizer.recognizing(this.privIntentRecognizer, ev);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                }

                break;
            case "speech.phrase":
                const simple: SimpleSpeechPhrase = SimpleSpeechPhrase.fromJSON(connectionMessage.textBody);

                result = new IntentRecognitionResult(
                    undefined,
                    requestSession.requestId,
                    EnumTranslation.implTranslateRecognitionResult(simple.RecognitionStatus),
                    simple.DisplayText,
                    simple.Duration,
                    simple.Offset,
                    undefined,
                    connectionMessage.textBody,
                    undefined);

                ev = new IntentRecognitionEventArgs(result, result.offset, requestSession.sessionId);

                const sendEvent: () => void = () => {
                    if (this.privRecognizerConfig.isContinuousRecognition) {
                        // For continuous recognition telemetry has to be sent for every phrase as per spec.
                        this.sendTelemetryData(requestSession.requestId, connection, requestSession.getTelemetry());
                    }

                    if (!!this.privIntentRecognizer.recognized) {
                        try {
                            this.privIntentRecognizer.recognized(this.privIntentRecognizer, ev);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
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
                };

                // If intent data was sent, the terminal result for this recognizer is an intent being found.
                // If no intent data was sent, the terminal event is speech recognition being successful.
                if (false === this.privIntentDataSent || ResultReason.NoMatch === ev.result.reason) {
                    sendEvent();
                } else {
                    // Squirrel away the args, when the response event arrives it will build upon them
                    // and then return
                    this.privPendingIntentArgs = ev;
                }

                break;
            case "response":
                // Response from LUIS
                if (this.privRecognizerConfig.isContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.sendTelemetryData(requestSession.requestId, connection, requestSession.getTelemetry());
                }

                ev = this.privPendingIntentArgs;
                this.privPendingIntentArgs = undefined;

                const intentResponse: IntentResponse = IntentResponse.fromJSON(connectionMessage.textBody);

                if (undefined === ev) {
                    // Odd... Not sure this can happen
                    ev = new IntentRecognitionEventArgs(new IntentRecognitionResult(), 0 /*TODO*/, requestSession.sessionId);
                }

                // If LUIS didn't return anything, send the existing event, else
                // modify it to show the match.
                // See if the intent found is in the list of intents asked for.
                let addedIntent: AddedLmIntent = this.privAddedLmIntents[intentResponse.topScoringIntent.intent];

                if (this.privUmbrellaIntent !== undefined) {
                    addedIntent = this.privUmbrellaIntent;
                }

                if (null !== intentResponse && addedIntent !== undefined) {
                    const intentId = addedIntent.intentName === undefined ? intentResponse.topScoringIntent.intent : addedIntent.intentName;
                    let reason = ev.result.reason;

                    if (undefined !== intentId) {
                        reason = ResultReason.RecognizedIntent;
                    }

                    // make sure, properties is set.
                    const properties = (undefined !== ev.result.properties) ?
                        ev.result.properties : new PropertyCollection();

                    properties.setProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult, connectionMessage.textBody);

                    ev = new IntentRecognitionEventArgs(
                        new IntentRecognitionResult(
                            intentId,
                            ev.result.resultId,
                            reason,
                            ev.result.text,
                            ev.result.duration,
                            ev.result.offset,
                            ev.result.errorDetails,
                            ev.result.json,
                            properties),
                        ev.offset,
                        ev.sessionId);
                }

                if (!!this.privIntentRecognizer.recognized) {
                    try {
                        this.privIntentRecognizer.recognized(this.privIntentRecognizer, ev);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                }

                // report result to promise.
                if (!!successCallback) {
                    try {
                        successCallback(ev.result);
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
        if (!!this.privIntentRecognizer.canceled) {
            const properties: PropertyCollection = new PropertyCollection();
            properties.setProperty(CancellationErrorCodePropertyName, CancellationErrorCode[CancellationErrorCode.ConnectionFailure]);

            const result: IntentRecognitionResult = new IntentRecognitionResult(
                undefined,
                requestId,
                ResultReason.Canceled,
                undefined,
                undefined,
                undefined,
                error,
                undefined,
                properties);

            const cancelEvent: IntentRecognitionCanceledEventArgs = new IntentRecognitionCanceledEventArgs(
                CancellationReason.Error,
                error,
                CancellationErrorCode.ConnectionFailure,
                undefined,
                undefined,
                sessionId);
            try {
                this.privIntentRecognizer.canceled(this.privIntentRecognizer, cancelEvent);
                /* tslint:disable:no-empty */
            } catch { }
        }

    }

}
