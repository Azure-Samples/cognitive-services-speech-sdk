//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import {
    IAuthentication,
    IConnectionFactory,
    PlatformConfig,
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechRecognitionEvent,
    TranslationConnectionFactory,
    TranslationFailedEvent,
    TranslationHypothesisEvent,
    TranslationServiceRecognizer,
    TranslationSimplePhraseEvent,
    TranslationSynthesisErrorEvent,
    TranslationSynthesisEvent,
} from "../common.speech/Exports";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    CancellationReason,
    KeywordRecognitionModel,
    PropertyCollection,
    PropertyId,
    Recognizer,
    SpeechRecognitionCanceledEventArgs,
    TranslationStatus,
    TranslationSynthesisResult,
    TranslationSynthesisResultEventArgs,
    TranslationTextResult,
    TranslationTextResultCanceledEventArgs,
    TranslationTextResultEventArgs,
} from "./Exports";
import { SpeechTranslationConfig, SpeechTranslationConfigImpl } from "./SpeechTranslationConfig";
import { Translation } from "./Translations";

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
        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationFromLanguage), PropertyId[PropertyId.SpeechServiceConnection_TranslationFromLanguage]);
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

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationFromLanguage);
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
            if (!this.reco) {
                return;
            }
            switch (event.Name) {
                case "RecognitionEndedEvent":
                    {
                        const recoEndedEvent: RecognitionEndedEvent = event as RecognitionEndedEvent;

                        if (recoEndedEvent.Status !== RecognitionCompletionStatus.Success) {
                            const errorEvent: TranslationTextResultCanceledEventArgs = new TranslationTextResultCanceledEventArgs(
                                recoEndedEvent.SessionId,
                                CancellationReason.Error,
                                recoEndedEvent.Error,
                                null);

                            if (this.canceled) {
                                this.canceled(this, errorEvent); // call error handler, if configured
                            }

                            if (!!err) {
                                err(recoEndedEvent.Error); // call error handler, if configured
                            }
                        }
                    }
                    break;

                case "TranslationSimplePhraseEvent":
                    {
                        const evResult = event as TranslationSimplePhraseEvent;
                        if (evResult.Result === undefined) {
                            if (!!err) {
                                err(TranslationStatus[TranslationStatus.Error]); // TODO: Need to capture and route better error from the service.
                                break;
                            }
                        }

                        const result: TranslationTextResultEventArgs = this.FireEventForResult(evResult);

                        if (!!this.recognized) {
                            this.recognized(this, result);
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
                            cb = undefined;
                        }
                    }
                    break;
                case "TranslationHypothesisEvent":
                    {
                        const evResult = event as TranslationHypothesisEvent;

                        const result: TranslationTextResultEventArgs = this.FireEventForResult(evResult);

                        if (!!this.recognizing) {
                            this.recognizing(this, result);
                        }

                    }
                    break;
                case "TranslationFailedEvent":
                    {
                        const evResult = event as TranslationFailedEvent;
                        const errorEvent: TranslationTextResultCanceledEventArgs = new TranslationTextResultCanceledEventArgs(
                            evResult.SessionId,
                            CancellationReason.Error,
                            evResult.Result.Translation.FailureReason,
                            null);

                        if (!!this.canceled) {
                            this.canceled(this, errorEvent);
                        }

                        if (!!err) {
                            err(evResult.Result.Translation.FailureReason);
                        }
                    }
                    break;
                case "TranslationSynthesisEvent":
                    {
                        const evResut: TranslationSynthesisEvent = event as TranslationSynthesisEvent;
                        const retEvent: TranslationSynthesisResultEventArgs = new TranslationSynthesisResultEventArgs();

                        retEvent.result = new TranslationSynthesisResult();
                        retEvent.result.audio = evResut.Result;
                        retEvent.sessionId = evResut.SessionId;

                        if (!!this.synthesizing) {
                            this.synthesizing(this, retEvent);
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
                            this.canceled(this, canceledResult);
                        }

                        if (!!err) {
                            err(evResult.Result.FailureReason);
                        }
                    }
                    break;
            }
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

        this.recognizeOnceAsync(
            (result: TranslationTextResult) => {
                // ignored
            },
            (message: string) => {
                if (!!err) {
                    err(message);
                }
            });

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

    private FireEventForResult(evResult: TranslationSimplePhraseEvent): TranslationTextResultEventArgs {
        const ev = new TranslationTextResultEventArgs();
        ev.sessionId = evResult.SessionId;

        ev.result = new TranslationTextResult();

        ev.result.translations = new Translation();
        for (const translation of evResult.Result.Translation.Translations) {
            ev.result.translations.set(translation.Language, translation.Text);
        }

        return ev;
    }
}
