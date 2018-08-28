import { Promise } from "../../common/Exports";
import { ISpeechProperties, KeywordRecognitionModel, RecognitionErrorEventArgs, RecognitionStatus, RecognizerBase } from "./Recognizer";
import { AudioInputStream, OutputFormat } from "./SpeechFactory";

// tslint:disable-next-line:max-classes-per-file
export class IntentRecognitionResultEventArgs {
    /**
     * Represents the intent recognition result.
     * @return Represents the intent recognition result.
     */
    public result: IntentRecognitionResult;

    /**
     * A String represents the session identifier.
     * @return A String represents the session identifier.
     */
    public sessionId: string;

    public properties: ISpeechProperties;
}

// tslint:disable-next-line:max-classes-per-file
export class IntentRecognitionResult {
    /**
     * A String that represents the intent identifier being recognized.
     * @return A String that represents the intent identifier being recognized.
     */
    public intentId: string;

    /**
     * A String that represents the intent including properties being recognized.
     * @return A String that represents the intent including properties being recognized.
     */
    public languageUnderstanding: string;

    /**
     * A String that represents the intent as json.
     * @return A String that represents the intent as json.
     */
    public json: string;

    /**
     * A String that represents error result in case the call failed.
     * @return A String that represents error result in case the call failed.
     */
    public errorDetails: string;
}

// tslint:disable-next-line:max-classes-per-file
export class LanguageUnderstandingModel {
    private constructor() {
    }

    /**
     * Creates an language understanding model using the specified endpoint.
     * @param uri A String that represents the endpoint of the language understanding model.
     * @return The language understanding model being created.
     */
    public static fromEndpoint(uri: URL): LanguageUnderstandingModel {
        //  Contracts.throwIfNullOrWhitespace(uri, "uri");
        //  return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromEndpoint(uri));
        return new LanguageUnderstandingModel();
    }

    /**
     * Creates an language understanding model using the application id of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @return The language understanding model being created.
     */
    public static fromAppId(appId: string): LanguageUnderstandingModel {
        //    Contracts.throwIfNullOrWhitespace(appId, "appId");
        //    return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromAppId(appId));
        return new LanguageUnderstandingModel();
    }
}

// tslint:disable-next-line:max-classes-per-file
export class IntentRecognizer extends RecognizerBase {
    private disposedIntentRecognizer: boolean = false;
    private intermediateResultReceivedHandler: Array<(sender: RecognizerBase, event: IntentRecognitionResultEventArgs) => void>;
    private finalResultReceivedHandler: Array<(sender: RecognizerBase, event: IntentRecognitionResultEventArgs) => void>;
    private recognitionErrorRaisedHandler: Array<(sender: RecognizerBase, event: RecognitionErrorEventArgs) => void>;

    // TODO should only be visible internally for SpeechFactory
    /**
     * Initializes an instance of the IntentRecognizer.
     * @param recoImpl The internal recognizer implementation.
     * @param ais An optional audio input stream associated with the recognizer
     */
    public constructor(parameters: ISpeechProperties, ais: AudioInputStream) {
        super(ais);

        this.parameters = parameters;
    }

    /**
     * The event IntermediateResultReceived signals that an intermediate recognition result is received.
     */
    public IntermediateResultReceived(onEventCallback: (sender: IntentRecognizer, event: IntentRecognitionResultEventArgs) => void): void {
        this.intermediateResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived(onEventCallback: (sender: IntentRecognizer, event: IntentRecognitionResultEventArgs) => void): void {
        this.finalResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised(onEventCallback: (sender: IntentRecognizer, event: RecognitionErrorEventArgs) => void): void {
        this.recognitionErrorRaisedHandler.push(onEventCallback);
    }

    /**
     * Gets the spoken language of recognition.
     * @return the spoken language of recognition.
     */
    public get language(): string {
        return this.parameters.get("language", "en-us");
    }

    /**
     * Sets the spoken language of recognition.
     * @param value the spoken language of recognition.
     */
    public set language(value: string) {
        // Contracts.throwIfNullOrWhitespace(value, "value");
        this.parameters.set("language", value);
    }

    /**
     * The collection of parameters and their values defined for this IntentRecognizer.
     * @return The collection of parameters and their values defined for this IntentRecognizer.
     */
    public parameters: ISpeechProperties;

    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of IntentRecognitionResult
     */
    public recognizeAsync(): Promise<IntentRecognitionResult> {
        //  return s_executorService.submit(() -> {
        //          return  new IntentRecognitionResult(recoImpl.Recognize());
        //      });
        return new Promise<IntentRecognitionResult>(null);
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public startContinuousRecognitionAsync(): Promise<void> {
        //  return s_executorService.submit(() -> {
        //          recoImpl.StartContinuousRecognition();
        //          return null;
        //      });
        return new Promise<void>(null);
    }

    /**
     * Stops continuous intent recognition.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public stopContinuousRecognitionAsync(): Promise<void> {
        //  return s_executorService.submit(() -> {
        //          recoImpl.StopContinuousRecognition();
        //          return null;
        //      });
        return new Promise<void>(null);
    }

    /**
     * Adds a phrase that should be recognized as intent.
     * @param intentId A String that represents the identifier of the intent to be recognized.
     * @param phrase A String that specifies the phrase representing the intent.
     */
    public addIntent(intentId: string, phrase: string): void {
        //  Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        //  Contracts.throwIfNullOrWhitespace(phrase, "phrase");

        //  recoImpl.AddIntent(intentId, phrase);
    }

    /**
     * Adds an intent from Language Understanding service for recognition.
     * @param intentId A String that represents the identifier of the intent to be recognized.
     * @param model The intent model from Language Understanding service.
     * @param intentName The intent name defined in the intent model. If it is null, all intent names defined in the model will be added.
     */
    public addIntentWithLanguageModel(intentId: string, model: LanguageUnderstandingModel, intentName: string): void {
        //  Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        //  Contracts.throwIfNullOrWhitespace(intentName, "intentName");
        //  Contracts.throwIfNull(model, "model");

        //  IntentTrigger trigger = com.microsoft.cognitiveservices.speech.internal.IntentTrigger.From(model.getModelImpl(), intentName);
        //  recoImpl.AddIntent(intentId, trigger);
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel): Promise<void> {
        //  Contracts.throwIfNull(model, "model");

        //  return s_executorService.submit(() -> {
        //          recoImpl.StartKeywordRecognition(model.getModelImpl());
        //          return null;
        //      });
        return new Promise<void>(null);
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public stopKeywordRecognitionAsync(): Promise<void> {
        //  return s_executorService.submit(() -> {
        //      recoImpl.StopKeywordRecognition();
        //      return null;
        //  });
        return new Promise<void>(null);
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
