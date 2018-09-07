//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { FileAudioSource } from "../../common.browser/Exports";
import { IAudioSource } from "../../common/Exports";
import {
    IAuthentication,
    IConnectionFactory,
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechConfig,
    SpeechRecognitionEvent,
    TranslationConfig,
    TranslationConnectionFactory,
    TranslationFailedEvent,
    TranslationHypothesisEvent,
    TranslationServiceRecognizer,
    TranslationSimplePhraseEvent,
    TranslationSynthesisErrorEvent,
    TranslationSynthesisEvent,
} from "../speech/Exports";
import { Contracts } from "./Contracts";
import {
    AudioInputStream,
    FactoryParameterNames,
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

export class TranslationRecognizer extends Recognizer {
    private disposedTranslationRecognizer: boolean;

    /**
     * Initializes an instance of the TranslationRecognizer.
     * @param recoImpl The internal recognizer implementation.
     * @param ais An optional audio input stream associated with the recognizer
     */
    public constructor(parameters: ISpeechProperties, ais: AudioInputStream) {
        super(ais);

        this.disposedTranslationRecognizer = false;
        this.parameters = parameters;
    }

    /**
     * The event IntermediateResultReceived signals that an intermediate recognition result is received.
     */
    public IntermediateResultReceived: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void;

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void;

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised: (sender: TranslationRecognizer, event: RecognitionErrorEventArgs) => void;

    /**
     * The event SynthesisResultReceived signals that a translation synthesis result is received.
     */
    public SynthesisResultReceived: (sender: TranslationRecognizer, event: TranslationSynthesisResultEventArgs) => void;

    /**
     * Gets the language name that was set when the recognizer was created.
     * @return Gets the language name that was set when the recognizer was created.
     */
    public get sourceLanguage(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.parameters.get(RecognizerParameterNames.TranslationFromLanguage, "en-us");
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @return Gets target languages for translation that were set when the recognizer was created.
     */
    public get targetLanguages(): string[] {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        let n = 0;
        const languages = [];

        while (this.parameters.has(RecognizerParameterNames.TranslationToLanguage + n)) {
            languages.push(this.parameters.get(RecognizerParameterNames.TranslationToLanguage + n, undefined));
            n++;
        }

        return languages;
    }

    /**
     * Gets the name of output voice.
     * @return the name of output voice.
     */
    public get outputVoiceName(): string {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        return this.parameters.get(RecognizerParameterNames.TranslationVoice, undefined);
    }

    /**
     * The collection of parameters and their values defined for this TranslationRecognizer.
     * @return The collection of parameters and their values defined for this TranslationRecognizer.
     */
    public parameters: ISpeechProperties;

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitableonly
     *       for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @param cb Callback that received the result when the translation has completed.
     * @param err Callback invoked in case of an error.
     */
    public recognizeAsync(cb?: (e: TranslationTextResult) => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        this.implCloseExistingRecognizer();

        let audioSource;
        if (this.audioInputStreamHolder) {
            audioSource = new FileAudioSource(this.audioInputStreamHolder.file);
        } else if (this.parameters.has("audioFile")) {
            const file = new File([""], this.parameters.get("audioFile", undefined));
            audioSource = new FileAudioSource(file);
        }

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Conversation,
            this.parameters,
            audioSource,
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

                        if (this.RecognitionErrorRaised) {
                            this.RecognitionErrorRaised(this, errorEvent); // call error handler, if configured
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

                        if (!!this.FinalResultReceived) {
                            this.FinalResultReceived(this, result);
                        }

                        // report result to promise.
                        if (!!cb) {
                            cb(result.result);
                            cb = undefined;
                        }
                    }
                    break;
                case "TranslationHypothesisEvent":
                    {
                        const evResult = event as TranslationHypothesisEvent;

                        const result: TranslationTextResultEventArgs = this.FireEventForResult(evResult);

                        if (!!this.IntermediateResultReceived) {
                            this.IntermediateResultReceived(this, result);
                        }

                    }
                    break;
                case "TranslationFailedEvent":
                    {
                        const evResult = event as TranslationFailedEvent;
                        const errorEvent: RecognitionErrorEventArgs = new RecognitionErrorEventArgs();
                        errorEvent.status = RecognitionStatus.Canceled;
                        errorEvent.sessionId = evResult.SessionId;

                        if (!!this.RecognitionErrorRaised) {
                            this.RecognitionErrorRaised(this, errorEvent);
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

                        if (!!this.SynthesisResultReceived) {
                            this.SynthesisResultReceived(this, retEvent);
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

                        if (!!this.SynthesisResultReceived) {
                            this.SynthesisResultReceived(this, retEvent);
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
     * Starts recognition and translation on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @param cb Callback that received the translation has started.
     * @param err Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        this.implCloseExistingRecognizer();

        this.recognizeAsync(
            (result: TranslationTextResult) => {
                // ignored
            },
            (message: string) => {
                if (!!err) {
                    err(message);
                }
            });

        if (!!cb) {
            cb();
            cb = undefined;
        }
    }

    /**
     * Stops continuous recognition and translation.
     * @param cb Callback that received the translation has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        this.implCloseExistingRecognizer();

        if (!!cb) {
            cb();
        }
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @param cb Callback that received the translation has started.
     * @param err Callback invoked in case of an error.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel, cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
        Contracts.throwIfNull(model, "model");

        //  return s_executorService.submit(() -> {
        //          recoImpl.StartKeywordRecognitionAsync(model.getModelImpl());
        //          return null;
        //      });

        this.implCloseExistingRecognizer();
        throw new Error("not supported");
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param cb Callback that received the translation has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedTranslationRecognizer);

        //  return s_executorService.submit(() -> {
        //          recoImpl.StopKeywordRecognitionAsync();
        //          return null;
        //      });

        this.implCloseExistingRecognizer();
        throw new Error("not supported");
    }

    /**
     * closes all external resources held by an instance of this class.
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

    protected CreateRecognizerConfig(speechConfig: SpeechConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new TranslationConfig(
            speechConfig,
            recognitionMode,
            this.parameters.get(RecognizerParameterNames.TranslationFromLanguage, "en-us"),
            this.targetLanguages,
            this.outputVoiceName,
            this.parameters);
    }

    protected CreateServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory, audioSource: IAudioSource, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {
        return new TranslationServiceRecognizer(authentication, connectionFactory, audioSource, recognizerConfig);
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
