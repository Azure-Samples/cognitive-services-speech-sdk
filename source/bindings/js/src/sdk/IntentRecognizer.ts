//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    EnumTranslation,
    IAuthentication,
    IConnectionFactory,
    IDetailedSpeechPhrase,
    IIntentResponse,
    IntentConnectionFactory,
    IntentServiceRecognizer,
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
        Contracts.throwIfNull(speechConfig, "speechConfig");
        const configImpl: SpeechConfigImpl = speechConfig as SpeechConfigImpl;
        Contracts.throwIfNull(configImpl, "speechConfig");

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
     * @property
     */
    public recognizing: (sender: IntentRecognizer, event: IntentRecognitionEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @property
     */
    public recognized: (sender: IntentRecognizer, event: IntentRecognitionEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @property
     */
    public canceled: (sender: IntentRecognizer, event: IntentRecognitionCanceledEventArgs) => void;

    /**
     * Gets the spoken language of recognition.
     * @property
     * @returns the spoken language of recognition.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public get authorizationToken(): string {
        return this.properties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @param value Authorization token.
     */
    public set authorizationToken(value: string) {
        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    /**
     * The collection of properties and their values defined for this IntentRecognizer.
     * @property
     * @returns The collection of properties and their values defined for this IntentRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member
     * @param cb - Callback that received the recognition has finished with an IntentRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: IntentRecognitionResult) => void, err?: (e: string) => void): void {
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
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @member
     * @param cb - Callback that received the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
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

    }

    /**
     * Stops continuous intent recognition.
     * @member
     * @param cb - Callback that received the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
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
    }

    /**
     * Adds a phrase that should be recognized as intent.
     * @member
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
     * @member
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
     * Adds all intents from the specified Language Understanding Model.
     * @param model The language understanding model containing the intents.
     * @param intentId A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    public addAllIntents(model: LanguageUnderstandingModel, intentId?: string): void {
        Contracts.throwIfNull(model, "model");

        const modelImpl: LanguageUnderstandingModelImpl = model as LanguageUnderstandingModelImpl;
        Contracts.throwIfNullOrWhitespace(modelImpl.appId, "model.appId");

        this.umbrellaIntent = new AddedLmIntent(modelImpl, intentId);
    }

    /**
     * closes all external resources held by an instance of this class.
     * @member
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
                        const result: IntentRecognitionResult = new IntentRecognitionResult();
                        result.errorDetails = RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error;
                        result.reason = ResultReason.Canceled;

                        const errorEvent: IntentRecognitionCanceledEventArgs = new IntentRecognitionCanceledEventArgs(
                            result,
                            recoEndedEvent.SessionId,
                            CancellationReason.Error,
                            recoEndedEvent.Error);

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
                    const result: IntentRecognitionResult = new IntentRecognitionResult();
                    result.json = JSON.stringify(evResult.Result);
                    result.text = evResult.Result.DisplayText;
                    result.duration = evResult.Result.Duration;
                    result.offset = evResult.Result.Offset;
                    result.reason = reason;

                    if (reason === ResultReason.Canceled) {
                        const ev = new IntentRecognitionCanceledEventArgs(
                            result,
                            evResult.SessionId,
                            EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus),
                            "");

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
                        const ev = new IntentRecognitionEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.result = result;

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
                            ev.result.reason = ResultReason.NoMatch;
                        }

                        // If intent data was sent, the terminal result for this redognizer is an intent being found.
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

                    const ev = new IntentRecognitionEventArgs();
                    ev.sessionId = evResult.SessionId;

                    ev.result = new IntentRecognitionResult();
                    ev.result.json = JSON.stringify(evResult.Result);

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
                        ev = new IntentRecognitionEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.result = new IntentRecognitionResult();
                    }

                    // If LUIS didn't return anything, send the existng event, else
                    // modify it to show the match.
                    // See if the intent found is in the list of intents asked for.

                    let addedIntent: AddedLmIntent = this.addedLmIntents[evResult.Result.topScoringIntent.intent];

                    if (this.umbrellaIntent !== undefined) {
                        addedIntent = this.umbrellaIntent;
                    }

                    if (null !== evResult.Result && addedIntent !== undefined) {

                        ev.result.intentId = addedIntent.intentName === undefined ? evResult.Result.topScoringIntent.intent : addedIntent.intentName;

                        if (undefined !== ev.result.intentId) {
                            ev.result.reason = ResultReason.RecognizedIntent;
                        }

                        ev.result.languageUnderstanding = JSON.stringify(evResult.Result);
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

// tslint:disable-next-line:max-classes-per-file
class AddedLmIntent {
    public modelImpl: LanguageUnderstandingModelImpl;
    public intentName: string;

    public constructor(modelImpl: LanguageUnderstandingModelImpl, intentName: string) {
        this.modelImpl = modelImpl;
        this.intentName = intentName;
    }
}
