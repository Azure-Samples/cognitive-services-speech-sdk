//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import {
    EnumTranslation,
    IAuthentication,
    IConnectionFactory,
    InternalErrorEvent,
    ISimpleSpeechPhrase,
    PlatformConfig,
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechRecognitionEvent,
    SpeechRecognitionResultEvent,
    TranslationConnectionFactory,
    TranslationFailedEvent,
    TranslationHypothesisEvent,
    TranslationPhraseEvent,
    TranslationServiceRecognizer,
    TranslationSynthesisErrorEvent,
    TranslationSynthesisEvent,
} from "../common.speech/Exports";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    CancellationReason,
    PropertyCollection,
    PropertyId,
    Recognizer,
    ResultReason,
    TranslationRecognitionCanceledEventArgs,
    TranslationRecognitionEventArgs,
    TranslationRecognitionResult,
    Translations,
    TranslationSynthesisEventArgs,
    TranslationSynthesisResult,
} from "./Exports";
import { SpeechTranslationConfig, SpeechTranslationConfigImpl } from "./SpeechTranslationConfig";

/**
 * Translation recognizer
 * @class TranslationRecognizer
 */
export class TranslationRecognizer extends Recognizer {
    private disposedTranslationRecognizer: boolean;
    private privProperties: PropertyCollection;

    /**
     * Initializes an instance of the TranslationRecognizer.
     * @constructor
     * @param {SpeechTranslationConfig} speechConfig - Set of properties to configure this recognizer.
     * @param {AudioConfig} audioConfig - An optional audio config associated with the recognizer
     */
    public constructor(speechConfig: SpeechTranslationConfig, audioConfig?: AudioConfig) {
        const configImpl = speechConfig as SpeechTranslationConfigImpl;
        Contracts.throwIfNull(configImpl, "speechConfig");

        super(audioConfig);

        this.disposedTranslationRecognizer = false;
        this.privProperties = configImpl.properties.clone();

        if (this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
            Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice), PropertyId[PropertyId.SpeechServiceConnection_TranslationVoice]);
        }

        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages), PropertyId[PropertyId.SpeechServiceConnection_TranslationToLanguages]);
        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage), PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);
    }

    /**
     * The event recognizing signals that an intermediate recognition result is received.
     * @member TranslationRecognizer.prototype.recognizing
     * @function
     * @public
     */
    public recognizing: (sender: TranslationRecognizer, event: TranslationRecognitionEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @member TranslationRecognizer.prototype.recognized
     * @function
     * @public
     */
    public recognized: (sender: TranslationRecognizer, event: TranslationRecognitionEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @member TranslationRecognizer.prototype.canceled
     * @function
     * @public
     */
    public canceled: (sender: TranslationRecognizer, event: TranslationRecognitionCanceledEventArgs) => void;

    /**
     * The event synthesizing signals that a translation synthesis result is received.
     * @member TranslationRecognizer.prototype.synthesizing
     * @function
     * @public
     */
    public synthesizing: (sender: TranslationRecognizer, event: TranslationSynthesisEventArgs) => void;

    /**
     * Gets the language name that was set when the recognizer was created.
     * @member TranslationRecognizer.prototype.speechRecognitionLanguage
     * @function
     * @public
     * @returns {string} Gets the language name that was set when the recognizer was created.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @member TranslationRecognizer.prototype.targetLanguages
     * @function
     * @public
     * @returns {string[]} Gets target languages for translation that were set when the recognizer was created.
     */
    public get targetLanguages(): string[] {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
    }

    /**
     * Gets the name of output voice.
     * @member TranslationRecognizer.prototype.voiceName
     * @function
     * @public
     * @returns {string} the name of output voice.
     */
    public get voiceName(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @member TranslationRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @returns {string} Authorization token.
     */
    public get authorizationToken(): string {
        return this.properties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @member TranslationRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @param {string} value - Authorization token.
     */
    public set authorizationToken(value: string) {
        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    /**
     * The collection of properties and their values defined for this TranslationRecognizer.
     * @member TranslationRecognizer.prototype.properties
     * @function
     * @public
     * @returns {PropertyCollection} The collection of properties and their values defined for this TranslationRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: recognizeOnceAsync returns when the first utterance has been recognized, so it is suitableonly
     *       for single shot recognition like command or query. For long-running recognition, use startContinuousRecognitionAsync() instead.
     * @member TranslationRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the result when the translation has completed.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: TranslationRecognitionResult) => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

            this.implCloseExistingRecognizer();

            this.reco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new TranslationConnectionFactory());

            this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
                if (this.disposedTranslationRecognizer || !this.reco) {
                    return;
                }

                this.implDispatchMessageHandler(event, cb, err);
            });
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
     * Starts recognition and translation, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @member TranslationRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback that received the translation has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

            this.implCloseExistingRecognizer();

            this.reco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new TranslationConnectionFactory());

            this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
                if (this.disposedTranslationRecognizer || !this.reco) {
                    return;
                }

                this.implDispatchMessageHandler(event, undefined, undefined);
            });

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
     * Stops continuous recognition and translation.
     * @member TranslationRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback that received the translation has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

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
     * closes all external resources held by an instance of this class.
     * @member TranslationRecognizer.prototype.close
     * @function
     * @public
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        this.dispose(true);
    }

    protected dispose(disposing: boolean): boolean {
        if (this.disposedTranslationRecognizer) {
            return;
        }

        if (disposing) {
            this.implCloseExistingRecognizer();
            this.disposedTranslationRecognizer = true;
            super.dispose(disposing);
        }
    }

    protected CreateRecognizerConfig(speechConfig: PlatformConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(speechConfig, RecognitionMode.Conversation, this.properties);
    }

    protected CreateServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory, audioConfig: AudioConfig, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {

        const configImpl: AudioConfigImpl = audioConfig as AudioConfigImpl;

        return new TranslationServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig);
    }

    // tslint:disable-next-line:member-ordering
    private reco: ServiceRecognizerBase;

    private implCloseExistingRecognizer(): void {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    }

    private implDispatchMessageHandler(event: SpeechRecognitionEvent, cb?: (e: TranslationRecognitionResult) => void, err?: (e: string) => void): void {

        if (!this.reco) {
            return;
        }
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    const recoEndedEvent: RecognitionEndedEvent = event as RecognitionEndedEvent;

                    if (recoEndedEvent.Status !== RecognitionCompletionStatus.Success) {
                        const result = new TranslationRecognitionResult(
                            undefined, undefined,
                            ResultReason.Canceled,
                            undefined, undefined, undefined,
                            RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error,
                            undefined, undefined);

                        const errorEvent: TranslationRecognitionCanceledEventArgs = new TranslationRecognitionCanceledEventArgs(
                            recoEndedEvent.SessionId,
                            CancellationReason.Error,
                            recoEndedEvent.Error,
                            result);

                        if (!!this.canceled) {
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
                                cb(errorEvent.result);
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
                }
                break;
            case "RecognitionFailedEvent":
                const evResult = event as SpeechRecognitionResultEvent<ISimpleSpeechPhrase>;

                const reason = EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                const result = new TranslationRecognitionResult(
                    undefined, undefined,
                    reason,
                    evResult.Result.DisplayText,
                    evResult.Result.Duration,
                    evResult.Result.Offset,
                    undefined,
                    JSON.stringify(evResult.Result),
                    undefined);

                if (reason === ResultReason.Canceled) {
                    const ev = new TranslationRecognitionCanceledEventArgs(
                        evResult.SessionId,
                        EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus),
                        null,
                        result);

                    if (!!this.canceled) {
                        try {
                            this.canceled(this, ev);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                } else {
                    const ev = new TranslationRecognitionEventArgs(result, 0/*offset*/, evResult.SessionId);

                    if (!!this.recognized) {
                        try {
                            this.recognized(this, ev);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
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
                    // and if it's successful don't invoke the
                    // error after that.
                    cb = undefined;
                    err = undefined;
                }

                break;
            case "TranslationPhraseEvent":
                {
                    const evResult = event as TranslationPhraseEvent;
                    const result: TranslationRecognitionEventArgs = this.FireEventForResult(evResult);

                    if (!!this.recognized) {
                        try {
                            this.recognized(this, result);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }

                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result.result);
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
            case "TranslationHypothesisEvent":
                {
                    const evResult = event as TranslationHypothesisEvent;

                    const result: TranslationRecognitionEventArgs = this.FireEventForResult(evResult);

                    if (!!this.recognizing) {
                        try {
                            this.recognizing(this, result);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }

                }
                break;
            case "TranslationFailedEvent":
                {
                    const evResult = event as TranslationFailedEvent;

                    const result = new TranslationRecognitionResult(
                        undefined, undefined,
                        ResultReason.RecognizedSpeech,
                        evResult.Result.Text,
                        evResult.Result.Duration,
                        evResult.Result.Offset,
                        undefined, undefined, undefined);

                    const retEvent: TranslationRecognitionEventArgs = new TranslationRecognitionEventArgs(result, 0 /*todo*/, evResult.SessionId);

                    if (!!this.recognized) {
                        try {
                            this.recognized(this, retEvent);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }

                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(retEvent.result);
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
            case "TranslationSynthesisEvent":
                {
                    const evResut: TranslationSynthesisEvent = event as TranslationSynthesisEvent;

                    const audio = evResut.Result;
                    const reason = (undefined === audio) ? ResultReason.SynthesizingAudioCompleted : ResultReason.SynthesizingAudio;
                    const result = new TranslationSynthesisResult(reason, audio);
                    const retEvent: TranslationSynthesisEventArgs = new TranslationSynthesisEventArgs(result, evResut.SessionId);

                    if (!!this.synthesizing) {
                        try {
                            this.synthesizing(this, retEvent);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "TranslationSynthesisErrorEvent":
                {
                    const evResult: TranslationSynthesisErrorEvent = event as TranslationSynthesisErrorEvent;

                    const result = new TranslationSynthesisResult(evResult.Result.SynthesisStatus, undefined);
                    const retEvent: TranslationSynthesisEventArgs = new TranslationSynthesisEventArgs(result, evResult.SessionId);

                    if (!!this.synthesizing) {
                        this.synthesizing(this, retEvent);
                    }

                    if (!!this.canceled) {
                        // And raise a canceled event to send the rich(er) error message back.
                        const canceledResult: TranslationRecognitionCanceledEventArgs = new TranslationRecognitionCanceledEventArgs(
                            evResult.SessionId,
                            CancellationReason.Error,
                            evResult.Result.FailureReason,
                            null);

                        try {
                            this.canceled(this, canceledResult);
                            /* tslint:disable:no-empty */
                        } catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "InternalErrorEvent":
                {
                    const evResult: InternalErrorEvent = event as InternalErrorEvent;
                    const result: TranslationRecognitionResult = new TranslationRecognitionResult(
                        undefined,
                        evResult.RequestId,
                        ResultReason.Canceled,
                        undefined,
                        undefined,
                        undefined,
                        evResult.Result);
                    const canceledResult: TranslationRecognitionCanceledEventArgs = new TranslationRecognitionCanceledEventArgs(
                        evResult.SessionId,
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

    private FireEventForResult(evResult: TranslationPhraseEvent | TranslationHypothesisEvent): TranslationRecognitionEventArgs {
        const translations = new Translations();
        for (const translation of evResult.Result.Translation.Translations) {
            translations.set(translation.Language, translation.Text);
        }

        const result = new TranslationRecognitionResult(
            translations,
            undefined,
            evResult instanceof TranslationPhraseEvent ? ResultReason.TranslatedSpeech : ResultReason.TranslatingSpeech,
            evResult.Result.Text,
            evResult.Result.Duration,
            evResult.Result.Offset,
            undefined,
            JSON.stringify(evResult.Result),
            undefined);

        const ev = new TranslationRecognitionEventArgs(result, 0, evResult.SessionId);
        return ev;
    }
}
