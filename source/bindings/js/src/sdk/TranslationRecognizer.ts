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
    TranslationConfig,
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
    ISpeechProperties,
    KeywordRecognitionModel,
    RecognitionErrorEventArgs,
    RecognitionStatus,
    Recognizer,
    RecognizerParameterNames,
    TranslationStatus,
    TranslationSynthesisResult,
    TranslationSynthesisResultEventArgs,
    TranslationTextResult,
    TranslationTextResultEventArgs,
} from "./Exports";
import { SpeechConfig, SpeechConfigImpl } from "./SpeechConfig";

/**
 * Translation recognizer
 * @class
 */
export class TranslationRecognizer extends Recognizer {
    private disposedTranslationRecognizer: boolean;

    /**
     * Initializes an instance of the TranslationRecognizer.
     * @constructor
     * @param {ISpeechProperties} parameters - The internal recognizer implementation.
     * @param {AudioConfig} ais - An optional audio input stream associated with the recognizer
     */
    public constructor(speechConfig: SpeechConfig, audioConfig?: AudioConfig) {
        super(speechConfig, audioConfig);

        this.disposedTranslationRecognizer = false;
        this.parameters = (speechConfig as SpeechConfigImpl).parameters;

        if (this.parameters.has(RecognizerParameterNames.TranslationVoice)) {
            Contracts.throwIfNullOrWhitespace(this.parameters.get(RecognizerParameterNames.TranslationVoice), RecognizerParameterNames.TranslationVoice);
        }

        Contracts.throwIfNullOrWhitespace(this.parameters.get(RecognizerParameterNames.TranslationToLanguage), RecognizerParameterNames.TranslationToLanguage);
        Contracts.throwIfNullOrWhitespace(this.parameters.get(RecognizerParameterNames.TranslationFromLanguage), RecognizerParameterNames.TranslationFromLanguage);
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
    public canceled: (sender: TranslationRecognizer, event: RecognitionErrorEventArgs) => void;

    /**
     * The event synthesized signals that a translation synthesis result is received.
     * @property
     */
    public synthesized: (sender: TranslationRecognizer, event: TranslationSynthesisResultEventArgs) => void;

    /**
     * Gets the language name that was set when the recognizer was created.
     * @property
     * @returns Gets the language name that was set when the recognizer was created.
     */
    public get sourceLanguage(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.parameters.get(RecognizerParameterNames.TranslationFromLanguage);
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @property
     * @returns Gets target languages for translation that were set when the recognizer was created.
     */
    public get targetLanguages(): string[] {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.parameters.get(RecognizerParameterNames.TranslationToLanguage).split(",");
    }

    /**
     * Gets the name of output voice.
     * @property
     * @returns the name of output voice.
     */
    public get outputVoiceName(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.parameters.get(RecognizerParameterNames.TranslationVoice, undefined);
    }

    /**
     * The collection of parameters and their values defined for this TranslationRecognizer.
     * @property
     * @returns The collection of parameters and their values defined for this TranslationRecognizer.
     */
    public parameters: ISpeechProperties;

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
            this.speechConfig,
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
                            const errorEvent: RecognitionErrorEventArgs = new RecognitionErrorEventArgs();

                            errorEvent.status = RecognitionStatus.Canceled;
                            errorEvent.sessionId = recoEndedEvent.SessionId;
                            errorEvent.error = recoEndedEvent.Error;

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
                                err(TranslationStatus.Error.toString()); // TODO: Need to capture and route better error from the service.
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
                        const errorEvent: RecognitionErrorEventArgs = new RecognitionErrorEventArgs();
                        errorEvent.status = RecognitionStatus.Canceled;
                        errorEvent.sessionId = evResult.SessionId;

                        if (!!this.canceled) {
                            this.canceled(this, errorEvent);
                        }

                        if (!!err) {
                            err(errorEvent.status.toString());
                        }
                    }
                    break;
                case "TranslationSynthesisEvent":
                    {
                        const evResut: TranslationSynthesisEvent = event as TranslationSynthesisEvent;
                        const retEvent: TranslationSynthesisResultEventArgs = new TranslationSynthesisResultEventArgs();

                        retEvent.result = new TranslationSynthesisResult();
                        retEvent.result.audio = new Uint8Array(evResut.Result);
                        retEvent.sessionId = evResut.SessionId;

                        if (!!this.synthesized) {
                            this.synthesized(this, retEvent);
                        }
                    }
                    break;
                case "TranslationSynthesisErrorEvent":
                    {
                        const evResut: TranslationSynthesisErrorEvent = event as TranslationSynthesisErrorEvent;
                        const retEvent: TranslationSynthesisResultEventArgs = new TranslationSynthesisResultEventArgs();

                        retEvent.result = new TranslationSynthesisResult();
                        retEvent.result.synthesisStatus = evResut.Result.SynthesisStatus;
                        retEvent.result.failureReason = evResut.Result.FailureReason;
                        retEvent.sessionId = evResut.SessionId;

                        if (!!this.synthesized) {
                            this.synthesized(this, retEvent);
                        }

                        if (!!err) {
                            err(evResut.Result.FailureReason);
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
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @member
     * @param {KeywordRecognitionModel} model - The keyword recognition model that specifies the keyword to be recognized.
     * @param cb - Callback that received the translation has started.
     * @param err - Callback invoked in case of an error.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel, cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
        Contracts.throwIfNull(model, "model");

        this.implCloseExistingRecognizer();
        throw new Error("not supported");
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @member
     * @param cb - Callback that received the translation has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        this.implCloseExistingRecognizer();
        throw new Error("not supported");
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
        return new TranslationConfig(
            speechConfig,
            this.speechConfig as SpeechConfigImpl);
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
        ev.result.translationStatus = (TranslationStatus as any)[evResult.Result.Translation.TranslationStatus];

        ev.result.translations = new ISpeechProperties();
        for (const translation of evResult.Result.Translation.Translations) {
            ev.result.translations.set(translation.Language, translation.Text);
        }

        return ev;
    }
}
