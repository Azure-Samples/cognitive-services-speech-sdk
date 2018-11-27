// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    IAuthentication,
    IConnectionFactory,
    OutputFormatPropertyName,
    PlatformConfig,
    RecognitionMode,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechServiceRecognizer,
} from "../common.speech/Exports";
import { SpeechConnectionFactory } from "../common.speech/SpeechConnectionFactory";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    KeywordRecognitionModel,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    Recognizer,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionEventArgs,
    SpeechRecognitionResult,
} from "./Exports";
import { SpeechConfig, SpeechConfigImpl } from "./SpeechConfig";

/**
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 * @class SpeechRecognizer
 */
export class SpeechRecognizer extends Recognizer {
    private privDisposedSpeechRecognizer: boolean = false;
    private privProperties: PropertyCollection;

    /**
     * SpeechRecognizer constructor.
     * @constructor
     * @param {SpeechConfig} speechConfig - An set of initial properties for this recognizer
     * @param {AudioConfig} audioConfig - An optional audio configuration associated with the recognizer
     */
    public constructor(speechConfig: SpeechConfig, audioConfig?: AudioConfig) {
        super(audioConfig);

        const speechConfigImpl: SpeechConfigImpl = speechConfig as SpeechConfigImpl;
        Contracts.throwIfNull(speechConfigImpl, "speechConfig");
        this.privProperties = speechConfigImpl.properties.clone();

        Contracts.throwIfNullOrWhitespace(
            speechConfigImpl.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage),
            PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);
    }

    /**
     * The event recognizing signals that an intermediate recognition result is received.
     * @member SpeechRecognizer.prototype.recognizing
     * @function
     * @public
     */
    public recognizing: (sender: Recognizer, event: SpeechRecognitionEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @member SpeechRecognizer.prototype.recognized
     * @function
     * @public
     */
    public recognized: (sender: Recognizer, event: SpeechRecognitionEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @member SpeechRecognizer.prototype.canceled
     * @function
     * @public
     */
    public canceled: (sender: Recognizer, event: SpeechRecognitionCanceledEventArgs) => void;

    /**
     * Gets the endpoint id of a customized speech model that is used for speech recognition.
     * @member SpeechRecognizer.prototype.endpointId
     * @function
     * @public
     * @returns {string} the endpoint id of a customized speech model that is used for speech recognition.
     */
    public get endpointId(): string {
        Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_EndpointId, "00000000-0000-0000-0000-000000000000");
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @member SpeechRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @param {string} token - Authorization token.
     */
    public set authorizationToken(token: string) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @member SpeechRecognizer.prototype.authorizationToken
     * @function
     * @public
     * @returns {string} Authorization token.
     */
    public get authorizationToken(): string {
        return this.properties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Gets the spoken language of recognition.
     * @member SpeechRecognizer.prototype.speechRecognitionLanguage
     * @function
     * @public
     * @returns {string} The spoken language of recognition.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @member SpeechRecognizer.prototype.outputFormat
     * @function
     * @public
     * @returns {OutputFormat} The output format of recognition.
     */
    public get outputFormat(): OutputFormat {
        Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

        if (this.properties.getProperty(OutputFormatPropertyName, OutputFormat[OutputFormat.Simple]) === OutputFormat[OutputFormat.Simple]) {
            return OutputFormat.Simple;
        } else {
            return OutputFormat.Detailed;
        }
    }

    /**
     * The collection of properties and their values defined for this SpeechRecognizer.
     * @member SpeechRecognizer.prototype.properties
     * @function
     * @public
     * @returns {PropertyCollection} The collection of properties and their values defined for this SpeechRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts speech recognition, and stops after the first utterance is recognized.
     * The task returns the recognition text as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized,
     *       so it is suitable only for single shot recognition
     *       like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member SpeechRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the SpeechRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: SpeechRecognitionResult) => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

            this.implCloseExistingRecognizer();

            this.privReco = this.implRecognizerSetup(
                RecognitionMode.Interactive,
                this.properties,
                this.audioConfig,
                new SpeechConnectionFactory());

            this.implRecognizerStart(this.privReco, cb, err);
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
     * @member SpeechRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

            this.implCloseExistingRecognizer();

            this.privReco = this.implRecognizerSetup(
                RecognitionMode.Conversation,
                this.properties,
                this.audioConfig,
                new SpeechConnectionFactory());

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
     * Stops continuous speech recognition.
     * @member SpeechRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        try {
            Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

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
     * Starts speech recognition with keyword spotting, until
     * stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the
     *      Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member SpeechRecognizer.prototype.startKeywordRecognitionAsync
     * @function
     * @public
     * @param {KeywordRecognitionModel} model The keyword recognition model that
     *        specifies the keyword to be recognized.
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
     * Note: Key word spotting functionality is only available on the
     *       Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member SpeechRecognizer.prototype.stopKeywordRecognitionAsync
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
     * closes all external resources held by an instance of this class.
     * @member SpeechRecognizer.prototype.close
     * @function
     * @public
     */
    public close(): void {
        Contracts.throwIfDisposed(this.privDisposedSpeechRecognizer);

        this.dispose(true);
    }

    /**
     * Disposes any resources held by the object.
     * @member SpeechRecognizer.prototype.dispose
     * @function
     * @public
     * @param {boolean} disposing - true if disposing the object.
     */
    protected dispose(disposing: boolean): void {
        if (this.privDisposedSpeechRecognizer) {
            return;
        }

        if (disposing) {
            this.implCloseExistingRecognizer();
            this.privDisposedSpeechRecognizer = true;
        }

        super.dispose(disposing);
    }

    protected createRecognizerConfig(speechConfig: PlatformConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(
            speechConfig,
            recognitionMode,
            this.properties);
    }

    protected createServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory,
                                      audioConfig: AudioConfig, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {
        const configImpl: AudioConfigImpl = audioConfig as AudioConfigImpl;
        return new SpeechServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig, this);
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
