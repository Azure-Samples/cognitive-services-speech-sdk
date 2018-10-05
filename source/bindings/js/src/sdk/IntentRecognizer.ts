//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    EnumTranslation,
    IAuthentication,
    IConnectionFactory,
    IDetailedSpeechPhrase,
    IIntentResponse,
    IntentConnectionFactory,
    IntentServiceRecognizer,
    InternalErrorEvent,
    ISimpleSpeechPhrase,
    ISpeechHypothesis,
    PlatformConfig,
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognitionStatus2,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechRecognitionEvent,
    SpeechRecognitionResultEvent,
} from "../common.speech/Exports";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    CancellationReason,
    IntentRecognitionCanceledEventArgs,
    IntentRecognitionEventArgs,
    IntentRecognitionResult,
    KeywordRecognitionModel,
    LanguageUnderstandingModel,
    PropertyCollection,
    PropertyId,
    Recognizer,
    ResultReason,
    SpeechConfig,
} from "./Exports";
import { LanguageUnderstandingModelImpl } from "./LanguageUnderstandingModel";
import { SpeechConfigImpl } from "./SpeechConfig";

/**
 * Intent recognizer.
 * @class
 */
export class IntentRecognizer extends Recognizer {
    private disposedIntentRecognizer: boolean;
    private privProperties: PropertyCollection;
    private reco: ServiceRecognizerBase;
    private pendingIntentArgs: IntentRecognitionEventArgs;
    private addedIntents: string[][];
    private addedLmIntents: { [id: string]: AddedLmIntent; };
    private intentDataSent: boolean;
    private umbrellaIntent: AddedLmIntent;

    /**
     * Initializes an instance of the IntentRecognizer.
     * @constructor
     * @param {SpeechConfig} speechConfig - The set of configuration properties.
     * @param {AudioConfig} audioConfig - An optional audio input config associated with the recognizer
     */
    public constructor(speechConfig: SpeechConfig, audioConfig?: AudioConfig) {
        Contracts.throwIfNullOrUndefined(speechConfig, "speechConfig");
        const configImpl: SpeechConfigImpl = speechConfig as SpeechConfigImpl;
        Contracts.throwIfNullOrUndefined(configImpl, "speechConfig");

        super(audioConfig);

        this.intentDataSent = false;
        this.addedIntents = [];
        this.addedLmIntents = {};

        this.disposedIntentRecognizer = false;
        this.privProperties = configImpl.properties;

        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage), PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);
    }

    /**
     * The event recognizing signals that an intermediate recognition result is received.
     * @member IntentRecognizer.prototype.recognizing
     * @function
     * @public
     */
    public recognizing: (sender: IntentRecognizer, event: IntentRecognitionEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @member IntentRecognizer.prototype.recognized
     * @function
     * @public
     */
    public recognized: (sender: IntentRecognizer, event: IntentRecognitionEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @member IntentRecognizer.prototype.canceled
     * @function
     * @public
     */
    public canceled: (sender: IntentRecognizer, event: IntentRecognitionCanceledEventArgs) => void;

    /**
     * Gets the spoken language of recognition.
     * @member IntentRecognizer.prototype.speechRecognitionLanguage
     * @function
     * @public
     * @returns {string} the spoken language of recognition.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @member IntentRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @returns {string} Authorization token.
     */
    public get authorizationToken(): string {
        return this.properties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @member IntentRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @param {string} value - Authorization token.
     */
    public set authorizationToken(value: string) {
        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    /**
     * The collection of properties and their values defined for this IntentRecognizer.
     * @member IntentRecognizer.prototype.properties
     * @function
     * @public
     * @returns {PropertyCollection} The collection of properties and their values defined for this IntentRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member IntentRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the recognition has finished with an IntentRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: IntentRecognitionResult) => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedIntentRecognizer);

            this.implCloseExistingRecognizer();

            this.reco = this.implRecognizerSetup(
                RecognitionMode.Interactive,
                this.properties,
                this.audioConfig,
                new IntentConnectionFactory());

            let contextJson: string;

            if (Object.keys(this.addedLmIntents).length !== 0 || undefined !== this.umbrellaIntent) {
                contextJson = this.buildSpeechContext();
                this.intentDataSent = true;
            }

            this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
                if (this.disposedIntentRecognizer || !this.reco) {
                    return;
                }

                this.implDispatchMessageHandler(event, cb, err);
            }, contextJson);
        } catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    const typedError: Error = error as Error;
                    err(typedError.name + ": " + typedError.message);
                } else {
                    err(error);
                }
            }
        }
    }

    /**
     * Starts speech recognition, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @member IntentRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedIntentRecognizer);

            this.implCloseExistingRecognizer();

            this.reco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new IntentConnectionFactory());

            let contextJson: string;

            if (Object.keys(this.addedLmIntents).length !== 0) {
                contextJson = this.buildSpeechContext();
                this.intentDataSent = true;
            }

            this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
                if (this.disposedIntentRecognizer || !this.reco) {
                    return;
                }

                this.implDispatchMessageHandler(event, undefined, undefined);
            }, contextJson);

            // report result to promise.
            if (!!cb) {
                try {
                    cb();
                } catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
                cb = undefined;
            }
        } catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    const typedError: Error = error as Error;
                    err(typedError.name + ": " + typedError.message);
                } else {
                    err(error);
                }
            }
        }
    }

    /**
     * Stops continuous intent recognition.
     * @member IntentRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedIntentRecognizer);

            this.implCloseExistingRecognizer();

            if (!!cb) {
                try {
                    cb();
                } catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
            }
        } catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    const typedError: Error = error as Error;
                    err(typedError.name + ": " + typedError.message);
                } else {
                    err(error);
                }
            }
        }
    }

    /**
     * Starts speech recognition with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member IntentRecognizer.prototype.startKeywordRecognitionAsync
     * @function
     * @public
     * @param {KeywordRecognitionModel} model - The keyword recognition model that specifies the keyword to be recognized.
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel, cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfNull(model, "model");

        if (!!err) {
            err("Not yet implemented.");
        }
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member IntentRecognizer.prototype.stopKeywordRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        if (!!cb) {
            cb();
        }
    }

    /**
     * Adds a phrase that should be recognized as intent.
     * @member IntentRecognizer.prototype.addIntent
     * @function
     * @public
     * @param {string} intentId - A String that represents the identifier of the intent to be recognized.
     * @param {string} phrase - A String that specifies the phrase representing the intent.
     */
    public addIntent(simplePhrase: string, intentId?: string): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");

        this.addedIntents.push([intentId, simplePhrase]);
    }

    /**
     * Adds an intent from Language Understanding service for recognition.
     * @member IntentRecognizer.prototype.addIntentWithLanguageModel
     * @function
     * @public
     * @param {string} intentId - A String that represents the identifier of the intent to be recognized. Ignored if intentName is empty.
     * @param {string} model - The intent model from Language Understanding service.
     * @param {string} intentName - The intent name defined in the intent model. If it is empty, all intent names defined in the model will be added.
     */
    public addIntentWithLanguageModel(intentId: string, model: LanguageUnderstandingModel, intentName?: string): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNull(model, "model");

        const modelImpl: LanguageUnderstandingModelImpl = model as LanguageUnderstandingModelImpl;
        Contracts.throwIfNullOrWhitespace(modelImpl.appId, "model.appId");

        this.addedLmIntents[intentId] = new AddedLmIntent(modelImpl, intentName);
    }

    /**
     * @summary Adds all intents from the specified Language Understanding Model.
     * @member IntentRecognizer.prototype.addAllIntents
     * @function
     * @public
     * @function
     * @public
     * @param {LanguageUnderstandingModel} model - The language understanding model containing the intents.
     * @param {string} intentId - A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public addAllIntents(model: LanguageUnderstandingModel, intentId?: string): void {
        Contracts.throwIfNull(model, "model");

        const modelImpl: LanguageUnderstandingModelImpl = model as LanguageUnderstandingModelImpl;
        Contracts.throwIfNullOrWhitespace(modelImpl.appId, "model.appId");

        this.umbrellaIntent = new AddedLmIntent(modelImpl, intentId);
    }

    /**
     * closes all external resources held by an instance of this class.
     * @member IntentRecognizer.prototype.close
     * @function
     * @public
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        this.dispose(true);
    }

    protected CreateRecognizerConfig(speecgConfig: PlatformConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(speecgConfig, recognitionMode, this.properties);
    }
    protected CreateServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory, audioConfig: AudioConfig, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {
        const audioImpl: AudioConfigImpl = audioConfig as AudioConfigImpl;
        return new IntentServiceRecognizer(authentication, connectionFactory, audioImpl, recognizerConfig);
    }

    protected dispose(disposing: boolean): void {
        if (this.disposedIntentRecognizer) {
            return;
        }

        if (disposing) {
            this.disposedIntentRecognizer = true;
            super.dispose(disposing);
        }
    }

    private implDispatchMessageHandler(event: SpeechRecognitionEvent, cb?: (e: IntentRecognitionResult) => void, err?: (e: string) => void): void {
        /*
         Alternative syntax for typescript devs.
         if (event instanceof SDK.RecognitionTriggeredEvent)
        */
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    const recoEndedEvent: RecognitionEndedEvent = event as RecognitionEndedEvent;

                    if (recoEndedEvent.Status !== RecognitionCompletionStatus.Success) {
                        const result: IntentRecognitionResult = new IntentRecognitionResult(
                            undefined, undefined,
                            ResultReason.Canceled,
                            undefined, undefined, undefined,
                            RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error,
                            undefined, undefined);

                        const errorEvent: IntentRecognitionCanceledEventArgs = new IntentRecognitionCanceledEventArgs(
                            CancellationReason.Error,
                            recoEndedEvent.Error,
                            result,
                            0, recoEndedEvent.SessionId);

                        if (this.canceled) {
                            try {
                                this.canceled(this, errorEvent);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }

                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(result);
                                /* tslint:disable:no-empty */
                            } catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                        }
                    }
                    break;
                }
            case "SpeechSimplePhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISimpleSpeechPhrase>;

                    const reason = EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    const result: IntentRecognitionResult = new IntentRecognitionResult(
                        undefined,
                        undefined,
                        reason,
                        evResult.Result.DisplayText,
                        evResult.Result.Duration,
                        evResult.Result.Offset,
                        undefined,
                        JSON.stringify(evResult.Result),
                        undefined);

                    if (reason === ResultReason.Canceled) {
                        const ev = new IntentRecognitionCanceledEventArgs(
                            EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus),
                            "",
                            result,
                            0, evResult.SessionId);

                        if (!!this.canceled) {
                            try {
                                this.canceled(this, ev);
                                /* tslint:disable:no-empty */
                            } catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }

                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(result);
                                /* tslint:disable:no-empty */
                            } catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                        }
                    } else {
                        let ev = new IntentRecognitionEventArgs(result, 0 /*TODO*/, evResult.SessionId);

                        const sendEvent: () => void = () => {
                            if (!!this.recognized) {
                                try {
                                    this.recognized(this, ev);
                                    /* tslint:disable:no-empty */
                                } catch (error) {
                                    // Not going to let errors in the event handler
                                    // trip things up.
                                }
                            }

                            // report result to promise.
                            if (!!cb) {
                                try {
                                    cb(ev.result);
                                } catch (e) {
                                    if (!!err) {
                                        err(e);
                                    }
                                }
                                // Only invoke the call back once.
                                // and if it's successful don't invoke the
                                // error after that.
                                cb = undefined;
                                err = undefined;
                            }
                        };

                        const status = (RecognitionStatus2 as any)[evResult.Result.RecognitionStatus];
                        if (status === RecognitionStatus2.InitialSilenceTimeout ||
                            status === RecognitionStatus2.BabbleTimeout) {
                            ev = new IntentRecognitionEventArgs(
                                new IntentRecognitionResult(
                                    ev.result.intentId,
                                    ev.result.resultId,
                                    ResultReason.NoMatch,
                                    ev.result.text,
                                    ev.result.duration,
                                    ev.result.offset,
                                    ev.result.errorDetails,
                                    ev.result.json,
                                    ev.result.properties),
                                ev.offset,
                                ev.sessionId);
                        }

                        // If intent data was sent, the terminal result for this recognizer is an intent being found.
                        // If no intent data was sent, the terminal event is speech recognition being successful.
                        if (false === this.intentDataSent || ResultReason.NoMatch === ev.result.reason) {
                            sendEvent();
                        } else {
                            // Squirrel away the args, when the response event arrives it will build upon them
                            // and then return
                            this.pendingIntentArgs = ev;

                            //// Also, set a one minute delay, if the response hasn't come back, or more speech events do, move on.
                            // setTimeout(() => {
                            //    if (undefined !== this.pendingIntentArgs && !this.disposedIntentRecognizer) {
                            //        sendEvent();
                            //        this.pendingIntentArgs = undefined;
                            //    }
                            // }, 60000);
                        }
                    }
                }
                break;
            case "SpeechHypothesisEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISpeechHypothesis>;

                    const result = new IntentRecognitionResult(
                        undefined,
                        undefined,
                        ResultReason.RecognizingIntent,
                        evResult.Result.Text,
                        evResult.Result.Duration,
                        evResult.Result.Offset,
                        undefined,
                        JSON.stringify(evResult.Result),
                        undefined);

                    const ev = new IntentRecognitionEventArgs(result, 0 /*TODO*/, evResult.SessionId);

                    if (!!this.recognizing) {
                        try {
                            this.recognizing(this, ev);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "IntentResponseEvent":
                {
                    let ev: IntentRecognitionEventArgs = this.pendingIntentArgs;
                    this.pendingIntentArgs = undefined;

                    const evResult: SpeechRecognitionResultEvent<IIntentResponse> = event as SpeechRecognitionResultEvent<IIntentResponse>;

                    if (undefined === ev) {
                        // Odd... Not sure this can happen
                        ev = new IntentRecognitionEventArgs(new IntentRecognitionResult(), 0 /*TODO*/, evResult.SessionId);
                    }

                    // If LUIS didn't return anything, send the existing event, else
                    // modify it to show the match.
                    // See if the intent found is in the list of intents asked for.

                    let addedIntent: AddedLmIntent = this.addedLmIntents[evResult.Result.topScoringIntent.intent];

                    if (this.umbrellaIntent !== undefined) {
                        addedIntent = this.umbrellaIntent;
                    }

                    if (null !== evResult.Result && addedIntent !== undefined) {
                        const intentId = addedIntent.intentName === undefined ? evResult.Result.topScoringIntent.intent : addedIntent.intentName;
                        let reason = ev.result.reason;

                        if (undefined !== intentId) {
                            reason = ResultReason.RecognizedIntent;
                        }

                        // make sure, properties is set.
                        const properties = (undefined !== ev.result.properties) ?
                            ev.result.properties : new PropertyCollection();

                        properties.setProperty(PropertyId.LanguageUnderstandingServiceResponse_JsonResult, JSON.stringify(evResult.Result));

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

                    if (!!this.recognized) {
                        try {
                            this.recognized(this, ev);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }

                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(ev.result);
                        } catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
            case "InternalErrorEvent":
                {
                    const evResult: InternalErrorEvent = event as InternalErrorEvent;
                    const result: IntentRecognitionResult = new IntentRecognitionResult(
                        undefined,
                        undefined,
                        ResultReason.Canceled,
                        undefined,
                        undefined,
                        undefined,
                        evResult.Result);
                    const canceledResult: IntentRecognitionCanceledEventArgs = new IntentRecognitionCanceledEventArgs(
                        CancellationReason.Error,
                        result.errorDetails,
                        result);

                    try {
                        this.canceled(this, canceledResult);
                        /* tslint:disable:no-empty */
                    } catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }

                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result);
                        } catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke thebundle
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
        }

    }

    private implCloseExistingRecognizer(): void {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    }

    private buildSpeechContext(): string {
        let appId: string;
        let region: string;
        let subscriptionKey: string;
        const refGrammers: string[] = [];

        if (undefined !== this.umbrellaIntent) {
            appId = this.umbrellaIntent.modelImpl.appId;
            region = this.umbrellaIntent.modelImpl.region;
            subscriptionKey = this.umbrellaIntent.modelImpl.subscriptionKey;
        }

        // Build the reference grammer array.
        for (const intentId of Object.keys(this.addedLmIntents)) {
            const addedLmIntent: AddedLmIntent = this.addedLmIntents[intentId];

            // validate all the same model, region, and key...
            if (appId === undefined) {
                appId = addedLmIntent.modelImpl.appId;
            } else {
                if (appId !== addedLmIntent.modelImpl.appId) {
                    throw new Error("Intents must all be from the same LUIS model");
                }
            }

            if (region === undefined) {
                region = addedLmIntent.modelImpl.region;
            } else {
                if (region !== addedLmIntent.modelImpl.region) {
                    throw new Error("Intents must all be from the same LUIS model in a single region");
                }
            }

            if (subscriptionKey === undefined) {
                subscriptionKey = addedLmIntent.modelImpl.subscriptionKey;
            } else {
                if (subscriptionKey !== addedLmIntent.modelImpl.subscriptionKey) {
                    throw new Error("Intents must all use the same subscription key");
                }
            }

            const grammer: string = "luis/" + appId + "-PRODUCTION#" + intentId;
            refGrammers.push(grammer);
        }

        return JSON.stringify({
            dgi: {
                ReferenceGrammars: (undefined === this.umbrellaIntent) ? refGrammers : ["luis/" + appId + "-PRODUCTION"],
            },
            intent: {
                id: appId,
                key: (subscriptionKey === undefined) ? this.privProperties.getProperty(PropertyId[PropertyId.SpeechServiceConnection_Key]) : subscriptionKey,
                provider: "LUIS",
            },
        });
    }
}

/**
 * @class AddedLmIntent
 */
// tslint:disable-next-line:max-classes-per-file
class AddedLmIntent {
    public modelImpl: LanguageUnderstandingModelImpl;
    public intentName: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param modelImpl - The model.
     * @param intentName - The intent name.
     */
    public constructor(modelImpl: LanguageUnderstandingModelImpl, intentName: string) {
        this.modelImpl = modelImpl;
        this.intentName = intentName;
    }
}
