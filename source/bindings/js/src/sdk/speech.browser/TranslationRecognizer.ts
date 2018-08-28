import { Promise } from "../../common/Exports";
import { ISpeechProperties, KeywordRecognitionModel, RecognitionErrorEventArgs, RecognitionStatus, RecognizerBase } from "./Recognizer";
import { AudioInputStream, OutputFormat } from "./SpeechFactory";

// tslint:disable-next-line:max-classes-per-file
export class TranslationTextResultEventArgs {
    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public result: TranslationTextResult;

    /**
     * Specifies the session identifier.
     * @return the session identifier.
     */
    public sessionId: string;
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationSynthesisResultEventArgs {
    /**
     * Specifies the translation synthesis result.
     * @return Specifies the translation synthesis result.
     */
    public result: TranslationSynthesisResult;

    /**
     * Specifies the session identifier.
     * @return Specifies the session identifier.
     */
    public sessionId: string;
}

/**
 * Defines recognition status.
 */
export enum TranslationStatus {
    Success = 0,
    Error,
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationTextResult {
    /**
     * Specifies translation status.
     * @return the translation status.
     */
    public translationStatus: TranslationStatus;

    /**
     * Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key
     * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
     * @return the current translation map.
     */
    public translations: ISpeechProperties;
}

export enum SynthesisStatus {
    /**
     * The response contains valid audio data.
     */
    Success,
    /**
     * Indicates the end of audio data. No valid audio data is included in the message.
     */
    SynthesisEnd,
    /**
     * Indicates an error occurred during synthesis data processing.
     */
    Error,
}

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 */
// tslint:disable-next-line:max-classes-per-file
export class TranslationSynthesisResult {
    /**
     * Translated text in the target language.
     * @return Translated text in the target language.
     */
    public audio: ByteString;

    /**
     * Status of the synthesis.
     * @return status of the synthesis.
     */
    public synthesisStatus: SynthesisStatus;
}

// tslint:disable-next-line:max-classes-per-file
export class TranslationRecognizer extends RecognizerBase {
    private disposedTranslationRecognizer: boolean = false;
    private intermediateResultReceivedHandler: Array<(sender: RecognizerBase, event: TranslationTextResultEventArgs) => void>;
    private finalResultReceivedHandler: Array<(sender: RecognizerBase, event: TranslationTextResultEventArgs) => void>;
    private recognitionErrorRaisedHandler: Array<(sender: RecognizerBase, event: RecognitionErrorEventArgs) => void>;
    private synthesisResultReceivedHandler: Array<(sender: RecognizerBase, event: TranslationSynthesisResultEventArgs) => void>;

    /**
     * Initializes an instance of the TranslationRecognizer.
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
    public IntermediateResultReceived(onEventCallback: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void): void {
        this.intermediateResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived(onEventCallback: (sender: TranslationRecognizer, event: TranslationTextResultEventArgs) => void): void {
        this.finalResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised(onEventCallback: (sender: TranslationRecognizer, event: RecognitionErrorEventArgs) => void): void {
        this.recognitionErrorRaisedHandler.push(onEventCallback);
    }

    /**
     * The event SynthesisResultReceived signals that a translation synthesis result is received.
     */
    public SynthesisResultReceived(onEventCallback: (sender: TranslationRecognizer, event: TranslationSynthesisResultEventArgs) => void): void {
        this.synthesisResultReceivedHandler.push(onEventCallback);
    }

    /**
     * Gets the language name that was set when the recognizer was created.
     * @return Gets the language name that was set when the recognizer was created.
     */
    public get sourceLanguage(): string {
        return this.parameters.get("sourceLanguage", "en-us");
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @return Gets target languages for translation that were set when the recognizer was created.
     */
    public get targetLanguages(): string[] {
        let n = 0;
        const languages = [];

        while (this.parameters.has("targetLanguages" + n)) {
            languages.push(this.parameters.get("targetLanguages" + n, undefined));
            n++;
        }

        return languages;
    }

    /**
     * Gets the name of output voice.
     * @return the name of output voice.
     */
    public get outputVoiceName(): string {
        return this.parameters.get("voice", "en-us");
    }

    /**
     * The collection of parameters and their values defined for this TranslationRecognizer.
     * @return The collection of parameters and their values defined for this TranslationRecognizer.
     */
    public parameters: ISpeechProperties;

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitableonly for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of TranslationTextResult.
     */
    public recognizeAsync(): Promise<TranslationTextResult> {
        //   return s_executorService.submit(() -> {
        //           return new TranslationTextResult(recoImpl.Recognize());
        //       });
        return new Promise<TranslationTextResult>(null);
    }

    /**
     * Starts recognition and translation on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public startContinuousRecognitionAsync(): Promise<void> {
        //   return s_executorService.submit(() -> {
        //           recoImpl.StartContinuousRecognitionAsync();
        //           return null;
        //       });
        return new Promise<void>(null);
    }

    /**
     * Stops continuous recognition and translation.
     * @return A task representing the asynchronous operation that stops the translation.
     */
    public stopContinuousRecognitionAsync(): Promise<void> {
        //   return s_executorService.submit(() -> {
        //           recoImpl.StopContinuousRecognitionAsync();
        //           return null;
        //       });
        return new Promise<void>(null);
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
        //          recoImpl.StartKeywordRecognitionAsync(model.getModelImpl());
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
        //          recoImpl.StopKeywordRecognitionAsync();
        //          return null;
        //      });
        return new Promise<void>(null);
    }

    protected dispose(disposing: boolean): boolean {
        if (this.disposedTranslationRecognizer) {
            return;
        }

        if (disposing) {
            //   intermediateResultHandler.delete();
            //   finalResultHandler.delete();
            //   errorHandler.delete();
            //   recoImpl.delete();
            //   _Parameters.close();
            this.disposedTranslationRecognizer = true;
            super.dispose(disposing);
        }
    }
}
