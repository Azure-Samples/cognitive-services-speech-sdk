package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.net.URI;
import java.util.ArrayList;
import java.io.File;
import java.io.IOException;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.util.Contracts;

 /**
   * Factory methods to create recognizers.
   */
 public final class SpeechFactory implements Closeable {

    // load the native library. Hold the class active so the
    // class GC does not reclaim it (and the local variables!)
    static Class<?> speechFactoryClass = null;
    static {

        // First choice is to load the binaries from
        // the resources attached to the jar file. On Android,
        // this is not possible, so falling back to loading it
        // from disk.
        try {
            // This is a loose coupling since the class might not be available
            // when running on Android. Therefore, use reflection to find the
            // helper class and call the load function.
            // Fall back to standard loadLibrary, in case that fails.
            Class<?> ncl = Class.forName("com.microsoft.cognitiveservices.speech.NativeLibraryLoader");
            java.lang.reflect.Method nclm = ncl.getMethod("loadNativeBinding");
            nclm.invoke(null); // static.
        }
        catch(java.lang.Error ex) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }
        catch(java.lang.Exception ex2) {
            // In case, we cannot load the helper class, fall back to loading
            // the binding just by binding name.
            // TODO name of library will depend on version
            System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
        }

        // prevent classgc from freeing this class
        speechFactoryClass = SpeechFactory.class;
    }

    /**
      * Creates an instance of recognizer factory.
      */
    private SpeechFactory(com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory factoryImpl) {
        Contracts.throwIfNull(factoryImpl, "factoryImpl");

        this.factoryImpl = factoryImpl;

        // connect the native properties with the swig layer.
        _Parameters = new ParameterCollection<SpeechFactory>(this);
    }

    /**
      * Static instance of SpeechFactory returned by passing subscriptionKey and service region.
      * @param subscriptionKey The subscription key.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return The speech factory
      */
    public static SpeechFactory fromSubscription(String subscriptionKey, String region) {
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromSubscription(subscriptionKey, region));
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
    public static SpeechFactory fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromAuthorizationToken(authorizationToken, region));
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
    public static SpeechFactory fromEndPoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");

        return new SpeechFactory(com.microsoft.cognitiveservices.speech.internal.SpeechFactory.FromEndpoint(endpoint.toString(), subscriptionKey));
    }

    /**
      * Gets the subscription key.
      * @return the subscription key.
      */
    public String getSubscriptionKey() {
        return _Parameters.getString(FactoryParameterNames.SubscriptionKey);
    }

    /**
      * Gets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @return Gets the authorization token.
      */
    public String getAuthorizationToken() {
        return _Parameters.getString(FactoryParameterNames.AuthorizationToken);
    }

    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setAuthorizationToken(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        _Parameters.set(FactoryParameterNames.AuthorizationToken, value);
    }

    /**
      * Gets the region name of the service to be connected.
      * @return the region name of the service to be connected.
      */
    public String getRegion() {
        return _Parameters.getString(FactoryParameterNames.Region);
    }

    /**
      * The collection of parameters and their values defined for this RecognizerFactory.
      * @return The collection of parameters and their values defined for this RecognizerFactory.
      */
    public ParameterCollection<SpeechFactory> getParameters() {
        return _Parameters;
    }

    private final ParameterCollection<SpeechFactory> _Parameters;

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer() {
        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(), null);
    }

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String language) {
        Contracts.throwIfIllegalLanguage(language, "language");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerFromConfig(language), null);
    }

    /**
      * Creates a speech recognizer, using the specified audio input configuration.
      * @param audioConfig Specifies the audio input configuration.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizerFromConfig(AudioConfig audioConfig) {
        Contracts.throwIfNull(audioConfig, "audioConfig");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerFromConfig(audioConfig.getConfigImpl()), audioConfig);
    }

    /**
     * Creates a speech recognizer, using the specified audio input configuration.
     * @param audioConfig Specifies the audio input configuration.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerFromConfig(AudioConfig audioConfig, String language) {
        Contracts.throwIfNull(audioConfig, "audioConfig");
        Contracts.throwIfIllegalLanguage(language, "language");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerFromConfig(language, 
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple,
                    audioConfig.getConfigImpl()
                    ), audioConfig);
    }
    /**
     * Creates a speech recognizer, using the specified audio input configuration.
     * @param audioConfig Specifies the audio input configuration.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerFromConfig(AudioConfig audioConfig, String language, OutputFormat format) {
        Contracts.throwIfNull(audioConfig, "audioConfig");
        Contracts.throwIfIllegalLanguage(language, "language");
        Contracts.throwIfNull(format, "format");

        return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerFromConfig(language,
                    format == OutputFormat.Simple ?
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed,
                    audioConfig.getConfigImpl()
                    ), audioConfig);
    }

    /**
      * Creates an intent recognizer, using the default microphone input.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer() {
        return new IntentRecognizer(factoryImpl.CreateIntentRecognizer(), null);
    }

    /**
      * Creates an intent recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer(String language) {
        Contracts.throwIfIllegalLanguage(language, "language");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerFromConfig(language), null);
    }

    /**
      * Creates an intent recognizer, using the specified audio input configuration.
      * @param audioConfig Specifies the audio input configuration.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerFromConfig(AudioConfig audioConfig) {
        Contracts.throwIfNull(audioConfig, "audioConfig");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerFromConfig(audioConfig.getConfigImpl()), audioConfig);
    }

    /**
      * Creates an intent recognizer, using the specified audio input configuration.
      * @param audioConfig Specifies the audio input configuration.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerFromConfig(AudioConfig audioConfig, String language) {
        Contracts.throwIfNull(audioConfig, "audioConfig");
        Contracts.throwIfIllegalLanguage(language, "language");

        return new IntentRecognizer(factoryImpl.CreateIntentRecognizerFromConfig(language, audioConfig.getConfigImpl()), audioConfig);
    }

    /**
      * Creates a translation recognizer, using the default microphone input.
      * @param sourceLanguage The spoken language that needs to be translated.
      * @param targetLanguages The language of translation.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizerFromConfig(String sourceLanguage, ArrayList<String> targetLanguages) {
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");

        com.microsoft.cognitiveservices.speech.internal.StringVector v = new com.microsoft.cognitiveservices.speech.internal.StringVector();
        
        for(String element : targetLanguages) {
            Contracts.throwIfIllegalLanguage(element, "targetLanguages");
            v.add(element);
        }

        return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerFromConfig(sourceLanguage, v), null);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerFromConfig(String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrWhitespace(voice, "voice");

        com.microsoft.cognitiveservices.speech.internal.StringVector v = new com.microsoft.cognitiveservices.speech.internal.StringVector();
       
        for(String element : targetLanguages) {
            Contracts.throwIfIllegalLanguage(element, "targetLanguages");
            v.add(element);
        }

        return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerFromConfig(sourceLanguage, v, voice), null);
   }
    
   /**
     * Creates a translation recognizer using the specified audio input configuration.
     * @param audioConfig Specifies the audio input configuration.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerFromConfig(AudioConfig audioConfig, String sourceLanguage, ArrayList<String> targetLanguages) {
        Contracts.throwIfNull(audioConfig, "audioConfig");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");

        com.microsoft.cognitiveservices.speech.internal.StringVector v = new com.microsoft.cognitiveservices.speech.internal.StringVector();
        for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
        }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerFromConfig(sourceLanguage, v, audioConfig.getConfigImpl()), audioConfig);
   }

    /**
     * Creates a translation recognizer using the specified audio input configuration.
     * @param audioConfig Specifies the audio input configuration.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerFromConfig(AudioConfig audioConfig, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
        Contracts.throwIfNull(audioConfig, "audioConfig");
        Contracts.throwIfIllegalLanguage(sourceLanguage, "sourceLanguage");
        Contracts.throwIfNull(targetLanguages, "targetLanguages");
        Contracts.throwIfNullOrWhitespace(voice, "voice");

        com.microsoft.cognitiveservices.speech.internal.StringVector v = new com.microsoft.cognitiveservices.speech.internal.StringVector();
        for(String element : targetLanguages) {
           Contracts.throwIfIllegalLanguage(element, "targetLanguages");
           v.add(element);
        }

       return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerFromConfig(sourceLanguage, v, voice, audioConfig.getConfigImpl()), audioConfig);
   }

    /**
      * Dispose of associated resources.
      */
    public void close() {
        if (disposed) {
            return;
        }

        _Parameters.close();
        factoryImpl.delete();
        disposed = true;
    }

    // TODO should only visible to property collection.
    public com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory getFactoryImpl() {
        return factoryImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory factoryImpl;
    private boolean disposed = false;
}
