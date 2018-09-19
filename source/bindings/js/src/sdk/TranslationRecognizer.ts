//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import {
    EnumTranslation,
    IAuthentication,
    IConnectionFactory,
    ISimpleSpeechPhrase,
    ITranslationPhrase,
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
    Translation,
    TranslationStatus,
    TranslationSynthesisResult,
    TranslationSynthesisResultEventArgs,
    TranslationTextResult,
    TranslationTextResultCanceledEventArgs,
    TranslationTextResultEventArgs,
} from "./Exports";
import { SpeechTranslationConfig, SpeechTranslationConfigImpl } from "./SpeechTranslationConfig";
import { SynthesisStatus } from "./SynthesisStatus";

/**
 * Translation recognizer
 * @class
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

        if (this.properties.hasProperty(PropertyId.SpeechServiceConnection_TranslationVoice)) {
            Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice), PropertyId[PropertyId.SpeechServiceConnection_TranslationVoice]);
        }

        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages), PropertyId[PropertyId.SpeechServiceConnection_TranslationToLanguages]);
        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage), PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);
    }

    /**
     * The event recognizing signals that an intermediate recognition result is received.
     * @property
     */
    public recognizing: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @property
     */
    public recognized: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @property
     */
    public canceled: (sender: TranslationRecognizer, event: TranslationTextResultCanceledEventArgs) => void;

    /**
     * The event synthesizing signals that a translation synthesis result is received.
     * @property
     */
    public synthesizing: (sender: TranslationRecognizer, event: TranslationSynthesisResultEventArgs) => void;

    /**
     * Gets the language name that was set when the recognizer was created.
     * @property
     * @returns Gets the language name that was set when the recognizer was created.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @property
     * @returns Gets target languages for translation that were set when the recognizer was created.
     */
    public get targetLanguages(): string[] {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
    }

    /**
     * Gets the name of output voice.
     * @property
     * @returns the name of output voice.
     */
    public get voiceName(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined);
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
     * The collection of properties and their values defined for this TranslationRecognizer.
     * @property
     * @returns The collection of properties and their values defined for this TranslationRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: recognizeOnceAsync returns when the first utterance has been recognized, so it is suitableonly
     *       for single shot recognition like command or query. For long-running recognition, use startContinuousRecognitionAsync() instead.
     * @member
     * @param cb - Callback that received the result when the translation has completed.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: TranslationTextResult) => void, err?: (e: string) => void): void {
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
    }

    /**
     * Starts recognition and translation on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @member
     * @param cb - Callback that received the translation has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
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

    }

    /**
     * Stops continuous recognition and translation.
     * @member
     * @param cb - Callback that received the translation has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
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
    }

    /**
     * closes all external resources held by an instance of this class.
     * @member
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
    private implDispatchMessageHandler(event: SpeechRecognitionEvent, cb?: (e: TranslationTextResult) => void, err?: (e: string) => void): void {

        if (!this.reco) {
            return;
        }
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    const recoEndedEvent: RecognitionEndedEvent = event as RecognitionEndedEvent;

                    if (recoEndedEvent.Status !== RecognitionCompletionStatus.Success) {
                        const result: TranslationTextResult = new TranslationTextResult();
                        result.reason = ResultReason.Canceled;
                        result.errorDetails = RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error;

                        const errorEvent: TranslationTextResultCanceledEventArgs = new TranslationTextResultCanceledEventArgs(
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
                const result: TranslationTextResult = new TranslationTextResult();
                result.json = JSON.stringify(evResult.Result);
                result.offset = evResult.Result.Offset;
                result.duration = evResult.Result.Duration;
                result.text = evResult.Result.DisplayText;
                result.reason = reason;

                if (reason === ResultReason.Canceled) {
                    const ev = new TranslationTextResultCanceledEventArgs(
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
                    const ev = new TranslationTextResultEventArgs();
                    ev.sessionId = evResult.SessionId;
                    ev.result = result;

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
                    const result: TranslationTextResultEventArgs = this.FireEventForResult(evResult);

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

                    const result: TranslationTextResultEventArgs = this.FireEventForResult(evResult);

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
                    const retEvent: TranslationTextResultEventArgs = new TranslationTextResultEventArgs();
                    retEvent.result = new TranslationTextResult();
                    retEvent.sessionId = evResult.SessionId;
                    retEvent.result.duration = evResult.Result.Duration;
                    retEvent.result.offset = evResult.Result.Offset;
                    retEvent.result.reason = ResultReason.RecognizedSpeech;
                    retEvent.result.text = evResult.Result.Text;

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
                    const retEvent: TranslationSynthesisResultEventArgs = new TranslationSynthesisResultEventArgs();

                    retEvent.result = new TranslationSynthesisResult();
                    retEvent.result.audio = evResut.Result;
                    retEvent.result.reason = (undefined === retEvent.result.audio) ? SynthesisStatus.SynthesisEnd : SynthesisStatus.Success;
                    retEvent.sessionId = evResut.SessionId;

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
                    const retEvent: TranslationSynthesisResultEventArgs = new TranslationSynthesisResultEventArgs();

                    retEvent.result = new TranslationSynthesisResult();
                    retEvent.result.reason = evResult.Result.SynthesisStatus;
                    retEvent.sessionId = evResult.SessionId;

                    if (!!this.synthesizing) {
                        this.synthesizing(this, retEvent);
                    }

                    if (!!this.canceled) {
                        // And raise a canceled event to send the rich(er) error message back.
                        const canceledResult: TranslationTextResultCanceledEventArgs = new TranslationTextResultCanceledEventArgs(
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
        }
    }

    private FireEventForResult(evResult: TranslationPhraseEvent | TranslationHypothesisEvent): TranslationTextResultEventArgs {
        const ev = new TranslationTextResultEventArgs();
        ev.sessionId = evResult.SessionId;

        ev.result = new TranslationTextResult();
        ev.result.duration = evResult.Result.Duration;
        ev.result.json = JSON.stringify(evResult.Result);
        ev.result.offset = evResult.Result.Offset;
        ev.result.reason = ResultReason.TranslatedSpeech;
        ev.result.text = evResult.Result.Text;

        ev.result.translations = new Translation();
        for (const translation of evResult.Result.Translation.Translations) {
            ev.result.translations.set(translation.Language, translation.Text);
        }

        return ev;
    }
}
