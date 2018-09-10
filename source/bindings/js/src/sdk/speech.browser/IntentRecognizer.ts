//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { IAuthentication, IConnectionFactory, RecognitionMode, RecognizerConfig, ServiceRecognizerBase, SpeechConfig } from "../speech/Exports";
import { Contracts } from "./Contracts";
import { AudioConfig, AudioInputStream, IntentRecognitionResult, IntentRecognitionResultEventArgs, ISpeechProperties, KeywordRecognitionModel, LanguageUnderstandingModel, RecognitionErrorEventArgs, Recognizer, RecognizerParameterNames } from "./Exports";

export class IntentRecognizer extends Recognizer {
    private disposedIntentRecognizer: boolean;

    // TODO should only be visible internally for SpeechFactory
    /**
     * Initializes an instance of the IntentRecognizer.
     * @param recoImpl The internal recognizer implementation.
     * @param ais An optional audio input stream associated with the recognizer
     */
    public constructor(parameters: ISpeechProperties, ais: AudioInputStream) {
        super(ais);

        this.disposedIntentRecognizer = false;
        this.parameters = parameters;
    }

    /**
     * The event IntermediateResultReceived signals that an intermediate recognition result is received.
     */
    public IntermediateResultReceived: (sender: IntentRecognizer, event: IntentRecognitionResultEventArgs) => void;

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived: (sender: IntentRecognizer, event: IntentRecognitionResultEventArgs) => void;

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised: (sender: IntentRecognizer, event: RecognitionErrorEventArgs) => void;

    /**
     * Gets the spoken language of recognition.
     * @return the spoken language of recognition.
     */
    public get language(): string {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        return this.parameters.get(RecognizerParameterNames.SpeechRecognitionLanguage, "en-us");
    }

    /**
     * Sets the spoken language of recognition.
     * @param value the spoken language of recognition.
     */
    public set language(value: string) {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.parameters.set("SPEECH-RecoLanguage", value);
    }

    /**
     * The collection of parameters and their values defined for this IntentRecognizer.
     * @return The collection of parameters and their values defined for this IntentRecognizer.
     */
    public parameters: ISpeechProperties;

    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @param cb Callback that received the recognition has finished with an IntentRecognitionResult.
     * @param err Callback invoked in case of an error.
     */
    public recognizeAsync(cb?: (e: IntentRecognitionResult) => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        //  return s_executorService.submit(() -> {
        //          return  new IntentRecognitionResult(recoImpl.Recognize());
        //      });
        throw new Error("not supported");
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @param cb Callback that received the recognition has started.
     * @param err Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        //  return s_executorService.submit(() -> {
        //          recoImpl.StartContinuousRecognition();
        //          return null;
        //      });
        throw new Error("not supported");
    }

    /**
     * Stops continuous intent recognition.
     * @param cb Callback that received the recognition has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        //  return s_executorService.submit(() -> {
        //          recoImpl.StopContinuousRecognition();
        //          return null;
        //      });
        throw new Error("not supported");
    }

    /**
     * Adds a phrase that should be recognized as intent.
     * @param intentId A String that represents the identifier of the intent to be recognized.
     * @param phrase A String that specifies the phrase representing the intent.
     */
    public addIntent(intentId: string, phrase: string): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNullOrWhitespace(phrase, "phrase");

        //  recoImpl.AddIntent(intentId, phrase);
        throw new Error("not supported");
    }

    /**
     * Adds an intent from Language Understanding service for recognition.
     * @param intentId A String that represents the identifier of the intent to be recognized. Ignored if intentName is empty.
     * @param model The intent model from Language Understanding service.
     * @param intentName The intent name defined in the intent model. If it is empty, all intent names defined in the model will be added.
     */
    public addIntentWithLanguageModel(intentId: string, model: LanguageUnderstandingModel, intentName: string): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts.throwIfNullOrWhitespace(intentName, "intentName");
        Contracts.throwIfNull(model, "model");

        //  IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.From(model.getModelImpl(), intentName);
        //  recoImpl.AddIntent(intentId, trigger);
        throw new Error("not supported");
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @param cb Callback that received the recognition has started.
     * @param err Callback invoked in case of an error.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel, cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts.throwIfNull(model, "model");

        //  return s_executorService.submit(() -> {
        //          recoImpl.StartKeywordRecognition(model.getModelImpl());
        //          return null;
        //      });
        throw new Error("not supported");
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param cb Callback that received the recognition has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        //  return s_executorService.submit(() -> {
        //      recoImpl.StopKeywordRecognition();
        //      return null;
        //  });
        throw new Error("not supported");
    }

    /**
     * closes all external resources held by an instance of this class.
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposedIntentRecognizer);

        this.dispose(true);
    }

    protected CreateRecognizerConfig(speecgConfig: SpeechConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        throw new Error("Method not implemented.");
    }
    protected CreateServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory, audioConfig: AudioConfig, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {
        throw new Error("Method not implemented.");
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
}
