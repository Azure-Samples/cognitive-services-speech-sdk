import { ArgumentNullError } from "../../common/Error";
import { IntentRecognizer } from "./IntentRecognizer";
import { ISpeechProperties } from "./Recognizer";
import { SpeechRecognizer } from "./SpeechRecognizer";
import { TranslationRecognizer } from "./TranslationRecognizer";

export enum OutputFormat {
    Simple = 0,
    Detailed,
}

// tslint:disable-next-line:max-classes-per-file
export class AudioInputStreamFormat {
    /**
     * The format of the audio, valid values: 1 (PCM)
     */
    public formatTag: number;

    /**
     * The number of channels, valid values: 1 (Mono).
     */
    public channels: number;

    /**
     * The sample rate, valid values: 16000.
     */
    public samplesPerSec: number;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     */
    public avgBytesPerSec: number;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     */
    public blockAlign: number;

    /**
     * The bits per sample, valid values: 16
     */
    public bitsPerSample: number;
}

// tslint:disable-next-line:max-classes-per-file
export class AudioInputStream {
    private fileHolder: File;

    protected constructor(file?: File) {
        this.fileHolder = file;
    }

    public get file(): File {
        return this.fileHolder;
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * @param dataBuffer The byte array to store the read data.
     * @return the number of bytes have been read.
     */
    public read(dataBuffer: number[]): number {
        // implement your own
        return -1;
    }

    /**
     * Returns the format of this audio stream.
     * @return The format of the audio stream.
     */
    public get format(): AudioInputStreamFormat {
        // implement your own
        return undefined;
    }

    /**
     * Closes the audio input stream.
     */
    public close(): void {
        // implement your own
    }
}

// tslint:disable-next-line:max-classes-per-file
export class FileInputStream extends AudioInputStream {

    public constructor(file: File) {
        super(file);
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechFactory {
    //  private ResultHandlerImpl intermediateResultHandler;
    //  private ResultHandlerImpl finalResultHandler;
    //  private ErrorHandlerImpl errorHandler;
    //  private factoryImpl: com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory;
    private disposed: boolean = false;

    /**
     * Static instance of SpeechFactory returned by passing subscriptionKey and service region.
     * @param subscriptionKey The subscription key.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech factory
     */
    public static fromSubscription = (subscriptionKey: string, region: string): SpeechFactory => {
        SpeechFactory.ThrowIfNullOrUndefined(subscriptionKey, "subscriptionKey");
        SpeechFactory.ThrowIfNullOrUndefined(region, "region");
        // return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromSubscription(subscriptionKey, region));

        if (subscriptionKey === undefined ||
            subscriptionKey === null) {
            throw new ArgumentNullError("subscriptionKey");
        }
        if (region === undefined ||
            region === null) {
            throw new ArgumentNullError("region");
        }

        const properties = new ISpeechProperties();
        properties.set("SPEECH-SubscriptionKey", subscriptionKey);
        properties.set("SPEECH-Region", region);

        return new SpeechFactory(properties);
    }

    /**
     * Static instance of SpeechFactory returned by passing authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expipres, the caller needs to refresh it by setting the property `AuthorizationToken` with a new valid token.
     * Otherwise, all the recognizers created by this SpeechFactory instance will encounter errors during recognition.
     * @param authorizationToken The authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech factory
     */
    public static fromAuthorizationToken = (authorizationToken: string, region: string): SpeechFactory => {
        SpeechFactory.ThrowIfNullOrUndefined(authorizationToken, "authorizationToken");
        SpeechFactory.ThrowIfNullOrUndefined(region, "region");

        // return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromAuthorizationToken(authorizationToken, region));
        const properties = new ISpeechProperties();
        properties.set("SPEECH-AuthToken", authorizationToken);
        properties.set("SPEECH-Region", region);

        return new SpeechFactory(properties);
    }

    /**
     * Creates an instance of the speech factory with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
     * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @return A speech factory instance.
     */
    public static fromEndpoint = (endpoint: URL, subscriptionKey: string): SpeechFactory => {
        SpeechFactory.ThrowIfNullOrUndefined(endpoint, "endpoint");
        SpeechFactory.ThrowIfNullOrUndefined(subscriptionKey, "subscriptionKey");

        // return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromEndpoint(endpoint.toString(), subscriptionKey));
        const properties = new ISpeechProperties();
        properties.set("SPEECH-SubscriptionKey", subscriptionKey);
        properties.set("SPEECH-Endpoint", endpoint.toString());

        return new SpeechFactory(properties);
    }

    // tslint:disable-next-line:member-ordering
    private constructor(properties: ISpeechProperties) {
        this.parameters = properties;
    }

    /**
     * Gets the subscription key.
     * @return the subscription key.
     */
    public get subscriptionKey(): string {
        return this.parameters.get("SPEECH-SubscriptionKey", undefined);
    }

    /**
     * Gets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @return Gets the authorization token.
     */
    public get authorizationToken(): string {
        return this.parameters.get("SPEECH-AuthToken", undefined);
    }

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @param value the authorization token.
     */
    public set authorizationToken(value: string) {
        SpeechFactory.ThrowIfNullOrUndefined(value, "value");

        this.parameters.set("SPEECH-AuthToken", value);
    }

    /**
     * Gets the region name of the service to be connected.
     * @return the region name of the service to be connected.
     */
    public get region(): string {
        return this.parameters.get("SPEECH-Region", undefined);
    }

    /**
     * Gets the service endpoint.
     * @return the service endpoint.
     */
    public get endpoint(): URL {
        // URI.create(_Parameters.getString(FactoryParameterNames.Endpoint));
        return this.parameters.has("SPEECH-Endpoint") ? new URL(this.parameters.get("SPEECH-Endpoint", undefined)) : undefined;
    }

    /**
     * The collection of parameters and their values defined for this RecognizerFactory.
     * @return The collection of parameters and their values defined for this RecognizerFactory.
     */
    public parameters: ISpeechProperties;

    /**
     * Creates a speech recognizer, using the default microphone input.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizer(): SpeechRecognizer {
        return new SpeechRecognizer(this.parameters.clone(), null); // new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(), null);
    }

    /**
     * Creates a speech recognizer, using the default microphone input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithLanguage(language: string): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");
        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the default microphone input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithLanguageAndOutput(language: string, format: OutputFormat): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");
        SpeechFactory.ThrowIfNullOrUndefined(format, "format");

        // return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language,
        //     format == OutputFormat.Simple ?
        //             com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
        //             com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
        //             ), null);
        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);
        parameters.set("SPEECH-OutputFormat", format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithFileInput(audioFile: string): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithFileInputAndLanguage(audioFile: string, language: string): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set("SPEECH-RecoLanguage", language);

        return new SpeechRecognizer(parameters, null);
    }
    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithFileInputAndLanguageAndOutput(audioFile: string, language: string, format: OutputFormat): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");
        SpeechFactory.ThrowIfNullOrUndefined(format, "format");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set("SPEECH-RecoLanguage", language);
        parameters.set("SPEECH-OutputFormat", format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithStream(audioStream: AudioInputStream): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        const parameters = this.parameters.clone();

        return new SpeechRecognizer(parameters, audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithStreamAndLanguage(audioStream: AudioInputStream, language: string): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);

        return new SpeechRecognizer(parameters, audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithStreamAndLanguageAndOutput(audioStream: AudioInputStream, language: string, format: OutputFormat): SpeechRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");
        SpeechFactory.ThrowIfNullOrUndefined(format, "format");

        //    return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.getAdapter(), language,
        //                format == OutputFormat.Simple ?
        //                 com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
        //                 com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
        //                 ), audioStream);
        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);
        parameters.set("SPEECH-OutputFormat", format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, audioStream);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizer(): IntentRecognizer {
        const parameters = this.parameters.clone();
        return new IntentRecognizer(parameters, null); // new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithLanguage(language: string): IntentRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);

        return new IntentRecognizer(parameters, null); // new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @return An intent recognizer instance
     */
    public createIntentRecognizerWithFileInput(audioFile: string): IntentRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);

        return new IntentRecognizer(parameters, null); // new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile), null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance
     */
    public createIntentRecognizerWithFileInputAndLanguage(audioFile: string, language: string): IntentRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set("SPEECH-RecoLanguage", language);

        return new IntentRecognizer(parameters, null); // new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
     * Creates an intent recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithStream(audioStream: AudioInputStream): IntentRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");

        const parameters = this.parameters.clone();

        return new IntentRecognizer(parameters, audioStream); // new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
     * Creates an intent recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithStreamAndLanguage(audioStream: AudioInputStream, language: string): IntentRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        SpeechFactory.ThrowIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("SPEECH-RecoLanguage", language);

        return new IntentRecognizer(parameters, audioStream); // new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language), null);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizer(sourceLanguage: string, targetLanguages: string[]): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        if (targetLanguages.length === 0) {
            throw new ArgumentNullError("targetLanguages");
        }

        // com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();

        const parameters = this.parameters.clone();
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        // return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, v), null);
        return new TranslationRecognizer(parameters, null);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizerWithVoice(sourceLanguage: string, targetLanguages: string[], voice: string): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        SpeechFactory.ThrowIfNullOrUndefined(voice, "voice");
        if (targetLanguages.length === 0) {
            throw new ArgumentNullError("targetLanguages");
        }

        const parameters = this.parameters.clone();
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);
        parameters.set("TRANSLATION-Voice", voice);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        // return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, v, voice), null);
        return new TranslationRecognizer(parameters, null);
    }

    /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizerWithFileInput(audioFile: string, sourceLanguage: string, targetLanguages: string[]): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        if (targetLanguages.length === 0) {
            throw new ArgumentNullError("targetLanguages");
        }

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        return new TranslationRecognizer(parameters, null);
    }

    /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizerWithFileInputAndVoice(audioFile: string, sourceLanguage: string, targetLanguages: string[], voice: string): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioFile, "audioFile");
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        SpeechFactory.ThrowIfNullOrUndefined(voice, "voice");
        if (targetLanguages.length === 0) {
            throw new ArgumentNullError("targetLanguages");
        }

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);
        parameters.set("TRANSLATION-Voice", voice);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        return new TranslationRecognizer(parameters, null);
    }

    /**
     * Creates a translation recognizer using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizerWithStream(audioStream: AudioInputStream, sourceLanguage: string, targetLanguages: string[]): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        if (targetLanguages.length === 0) {
            throw new ArgumentNullError("targetLanguages");
        }

        const parameters = this.parameters.clone();
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        return new TranslationRecognizer(parameters, audioStream);
    }

    /**
     * Creates a translation recognizer using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizerWithStreamAndVoice(audioStream: AudioInputStream, sourceLanguage: string, targetLanguages: string[], voice: string): TranslationRecognizer {
        SpeechFactory.ThrowIfNullOrUndefined(audioStream, "audioStream");
        SpeechFactory.ThrowIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        SpeechFactory.ThrowIfNullOrUndefined(targetLanguages, "targetLanguages");
        SpeechFactory.ThrowIfNullOrUndefined(voice, "voice");

        const parameters = this.parameters.clone();
        parameters.set("TRANSLATION-FromLanguage", sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set("TRANSLATION-ToLanguage" + n, targetLanguages[n]);
        }

        // return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.getAdapter(), sourceLanguage, v, voice), audioStream);
        return new TranslationRecognizer(parameters, audioStream);
    }

    /**
     * Dispose of associated resources.
     */
    public close(): void {
        if (!this.disposed) {
            // _Parameters.close();
            // factoryImpl.delete();
            this.disposed = true;
        }
    }

    private static ThrowIfNullOrUndefined(param: any, name: string): void {
        if (param === undefined || param === null) {
            throw new ArgumentNullError(name);
        }
    }
}
