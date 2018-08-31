//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { ArgumentNullError } from "../../common/Error";
import { Contracts } from "./Contracts";
import { AudioInputStream, FactoryParameterNames, IntentRecognizer, ISpeechProperties, OutputFormat, RecognizerParameterNames, SpeechRecognizer, TranslationRecognizer } from "./Exports";

 /**
  * Factory methods to create recognizers.
  */
export class SpeechFactory {
    private disposed: boolean;

    /**
     * Static instance of SpeechFactory returned by passing subscriptionKey and service region.
     * @param subscriptionKey The subscription key.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech factory
     */
    public static fromSubscription = (subscriptionKey: string, region: string): SpeechFactory => {
        Contracts.throwIfNullOrUndefined(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrUndefined(region, "region");

        if (subscriptionKey === undefined ||
            subscriptionKey === null) {
            throw new ArgumentNullError("subscriptionKey");
        }
        if (region === undefined ||
            region === null) {
            throw new ArgumentNullError("region");
        }

        const properties = new ISpeechProperties();
        properties.set(FactoryParameterNames.SubscriptionKey, subscriptionKey);
        properties.set(FactoryParameterNames.Region, region);

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
        Contracts.throwIfNullOrUndefined(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrUndefined(region, "region");

        const properties = new ISpeechProperties();
        properties.set(FactoryParameterNames.AuthorizationToken, authorizationToken);
        properties.set(FactoryParameterNames.Region, region);

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
        Contracts.throwIfNullOrUndefined(endpoint, "endpoint");
        Contracts.throwIfNullOrUndefined(subscriptionKey, "subscriptionKey");

        const properties = new ISpeechProperties();
        properties.set(FactoryParameterNames.SubscriptionKey, subscriptionKey);
        properties.set(FactoryParameterNames.Endpoint, endpoint.toString());

        return new SpeechFactory(properties);
    }

    // tslint:disable-next-line:member-ordering
    private constructor(properties: ISpeechProperties) {
        this.parameters = properties;
        this.disposed = false;
    }

    /**
     * Gets the subscription key.
     * @return the subscription key.
     */
    public get subscriptionKey(): string {
        Contracts.throwIfDisposed(this.disposed);

        return this.parameters.get(FactoryParameterNames.SubscriptionKey, undefined);
    }

    /**
     * Gets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @return Gets the authorization token.
     */
    public get authorizationToken(): string {
        Contracts.throwIfDisposed(this.disposed);

        return this.parameters.get(FactoryParameterNames.AuthorizationToken, undefined);
    }

    /**
     * Sets the authorization token.
     * If this is set, subscription key is ignored.
     * User needs to make sure the provided authorization token is valid and not expired.
     * @param value the authorization token.
     */
    public set authorizationToken(value: string) {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(value, "value");

        this.parameters.set(FactoryParameterNames.AuthorizationToken, value);
    }

    /**
     * Gets the region name of the service to be connected.
     * @return the region name of the service to be connected.
     */
    public get region(): string {
        Contracts.throwIfDisposed(this.disposed);

        return this.parameters.get(FactoryParameterNames.Region, undefined);
    }

    /**
     * Gets the service endpoint.
     * @return the service endpoint.
     */
    public get endpoint(): URL {
        Contracts.throwIfDisposed(this.disposed);

        return this.parameters.has(FactoryParameterNames.Endpoint) ? new URL(this.parameters.get(FactoryParameterNames.Endpoint, undefined)) : undefined;
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
        Contracts.throwIfDisposed(this.disposed);

        return new SpeechRecognizer(this.parameters.clone(), null);
    }

    /**
     * Creates a speech recognizer, using the default microphone input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithLanguage(language: string): SpeechRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the default microphone input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithLanguageAndOutput(language: string, format: OutputFormat): SpeechRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(language, "language");
        Contracts.throwIfNullOrUndefined(format, "format");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);
        parameters.set(RecognizerParameterNames.OutputFormat, format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithFileInput(audioFile: string): SpeechRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");
        Contracts.throwIfNullOrUndefined(language, "language");
        Contracts.throwIfNullOrUndefined(format, "format");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);
        parameters.set(RecognizerParameterNames.OutputFormat, format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, null);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @return A speech recognizer instance.
     */
    public createSpeechRecognizerWithStream(audioStream: AudioInputStream): SpeechRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");
        Contracts.throwIfNullOrUndefined(language, "language");
        Contracts.throwIfNullOrUndefined(format, "format");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);
        parameters.set(RecognizerParameterNames.OutputFormat, format === OutputFormat.Simple ? "SIMPLE" : "DETAILED");

        return new SpeechRecognizer(parameters, audioStream);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizer(): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);

        const parameters = this.parameters.clone();
        return new IntentRecognizer(parameters, null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithLanguage(language: string): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

        return new IntentRecognizer(parameters, null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @return An intent recognizer instance
     */
    public createIntentRecognizerWithFileInput(audioFile: string): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);

        return new IntentRecognizer(parameters, null);
    }

    /**
     * Creates an intent recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance
     */
    public createIntentRecognizerWithFileInputAndLanguage(audioFile: string, language: string): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

        return new IntentRecognizer(parameters, null);
    }

    /**
     * Creates an intent recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithStream(audioStream: AudioInputStream): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");

        const parameters = this.parameters.clone();

        return new IntentRecognizer(parameters, audioStream);
    }

    /**
     * Creates an intent recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance.
     */
    public createIntentRecognizerWithStreamAndLanguage(audioStream: AudioInputStream, language: string): IntentRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");
        Contracts.throwIfNullOrUndefined(language, "language");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.SpeechRecognitionLanguage, language);

        return new IntentRecognizer(parameters, audioStream);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @return A translation recognizer instance.
     */
    public createTranslationRecognizer(sourceLanguage: string, targetLanguages: string[]): TranslationRecognizer {
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
        }

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrUndefined(voice, "voice");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);
        parameters.set(RecognizerParameterNames.TranslationVoice, voice);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
        }

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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioFile, "audioFile");
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrUndefined(voice, "voice");

        const parameters = this.parameters.clone();
        parameters.set("audioFile", audioFile);
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);
        parameters.set(RecognizerParameterNames.TranslationVoice, voice);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
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
        Contracts.throwIfDisposed(this.disposed);
        Contracts.throwIfNullOrUndefined(audioStream, "audioStream");
        Contracts.throwIfNullOrUndefined(sourceLanguage, "sourceLanguage");
        Contracts.throwIfArrayEmptyOrWhitespace(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrUndefined(voice, "voice");

        const parameters = this.parameters.clone();
        parameters.set(RecognizerParameterNames.TranslationFromLanguage, sourceLanguage);
        parameters.set(RecognizerParameterNames.TranslationVoice, voice);

        for (let n = 0; n < targetLanguages.length; n++) {
            parameters.set(RecognizerParameterNames.TranslationToLanguage + n, targetLanguages[n]);
        }

        return new TranslationRecognizer(parameters, audioStream);
    }

    /**
     * Dispose of associated resources.
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposed);

        if (!this.disposed) {
            this.disposed = true;
        }
    }
}
