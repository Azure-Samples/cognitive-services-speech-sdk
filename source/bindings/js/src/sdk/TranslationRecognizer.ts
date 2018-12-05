// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    IAuthentication,
    IConnectionFactory,
    PlatformConfig,
    RecognitionMode,
    RecognizerConfig,
    ServiceRecognizerBase,
    TranslationConnectionFactory,
    TranslationServiceRecognizer,
} from "../common.speech/Exports";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    PropertyCollection,
    PropertyId,
    Recognizer,
    TranslationRecognitionCanceledEventArgs,
    TranslationRecognitionEventArgs,
    TranslationRecognitionResult,
    TranslationSynthesisEventArgs,
} from "./Exports";
import { SpeechTranslationConfig, SpeechTranslationConfigImpl } from "./SpeechTranslationConfig";

/**
 * Translation recognizer
 * @class TranslationRecognizer
 */
export class TranslationRecognizer extends Recognizer {
    private privDisposedTranslationRecognizer: boolean;
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

        this.privDisposedTranslationRecognizer = false;
        this.privProperties = configImpl.properties.clone();

        if (this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
            Contracts.throwIfNullOrWhitespace(
                this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice),
                PropertyId[PropertyId.SpeechServiceConnection_TranslationVoice]);
        }

        Contracts.throwIfNullOrWhitespace(
            this.properties.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages),
            PropertyId[PropertyId.SpeechServiceConnection_TranslationToLanguages]);

        Contracts.throwIfNullOrWhitespace(this.properties.getProperty(
            PropertyId.SpeechServiceConnection_RecoLanguage),
            PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);
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
        Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

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
        Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

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
        Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

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
     * Starts recognition and translation, and stops after the first utterance is recognized.
     * The task returns the translation text as result.
     * Note: recognizeOnceAsync returns when the first utterance has been recognized, so it is suitableonly
     *       for single shot recognition like command or query. For long-running recognition,
     *       use startContinuousRecognitionAsync() instead.
     * @member TranslationRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the result when the translation has completed.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: TranslationRecognitionResult) => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

            this.implCloseExistingRecognizer();

            this.privReco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new TranslationConnectionFactory());

            this.implRecognizerStart(
                this.privReco,
                (e: TranslationRecognitionResult) => {
                    this.implCloseExistingRecognizer();
                    if (!!cb) {
                        cb(e);
                    }
                }, (e: string) => {
                    this.implCloseExistingRecognizer();
                    if (!!err) {
                        err(e);
                    }
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
            Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

            this.implCloseExistingRecognizer();

            this.privReco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new TranslationConnectionFactory());

            this.implRecognizerStart(this.privReco, undefined, undefined);

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
            Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

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
        Contracts.throwIfDisposed(this.privDisposedTranslationRecognizer);

        this.dispose(true);
    }

    protected dispose(disposing: boolean): boolean {
        if (this.privDisposedTranslationRecognizer) {
            return;
        }

        if (disposing) {
            this.implCloseExistingRecognizer();
            this.privDisposedTranslationRecognizer = true;
            super.dispose(disposing);
        }
    }

    protected createRecognizerConfig(speechConfig: PlatformConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(speechConfig, RecognitionMode.Conversation, this.properties);
    }

    protected createServiceRecognizer(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioConfig: AudioConfig,
        recognizerConfig: RecognizerConfig): ServiceRecognizerBase {

        const configImpl: AudioConfigImpl = audioConfig as AudioConfigImpl;

        return new TranslationServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig, this);
    }

    // tslint:disable-next-line:member-ordering
    private privReco: ServiceRecognizerBase;

    private implCloseExistingRecognizer(): void {
        if (this.privReco) {
            this.privReco.audioSource.turnOff();
            this.privReco.dispose();
            this.privReco = undefined;
        }
    }
}
