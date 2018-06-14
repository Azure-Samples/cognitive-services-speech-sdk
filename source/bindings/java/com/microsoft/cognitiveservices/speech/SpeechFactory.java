package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.net.URI;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

 /**
   * Factory methods to create recognizers.
   */
 public final class SpeechFactory implements Closeable {
     
    // load the native library.
    static {
        // TODO name of library will depend on version
        System.loadLibrary("Microsoft.CognitiveServices.Speech.java.bindings");
    }

    /**
      * Creates an instance of recognizer factory with specified subscription key and region.
      * @param subscriptionKey The subscription key.
      * @param region The region name.
      */
    private SpeechFactory(String subscriptionKey, String region) {
        factoryImpl = com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromSubscription(subscriptionKey, region);

        // connect the native properties with the swig layer.
        _Parameters = new ParameterCollection<SpeechFactory>(this);
    }

    /**
      * Creates an instance of recognizer factory with specified subscription key and endpoint.
      * @param region The region name.
      * @param endpoint The endpoint.
      * @param subscriptionKey The subscription key or authenticationToken based on the isSubscription flag.
      */
    private SpeechFactory(java.net.URI endpoint, String subscriptionKey) {
        factoryImpl = com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromEndpoint(endpoint.toString(), subscriptionKey);

        // connect the native properties with the swig layer.
        _Parameters = new ParameterCollection<SpeechFactory>(this);
    }

    /**
      * Static instance of SpeechFactory returned by passing subscriptionKey 
      * @param subscriptionKey The subscription key.
      * @param region The region name. Pass null if not used.
      * @return The speech factory
      */
    public static SpeechFactory fromSubscription(String subscriptionKey, String region) {
        return new SpeechFactory(subscriptionKey, region);
    }    

    /**
      * Creates an instance of the speech factory with specified endpoint and subscription key.
      * This method is intended only for users who use a non-standard service endpoint.
      * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
      * For example, if language is defined in uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
      * the language setting in uri takes precedence, and the effective language is "de-DE".
      * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
      * @param endpoint The service endpoint to connect to.
      * @param subscriptionKey The subscription key.
      * @return A speech factory instance.
      */
    public static SpeechFactory fromEndPoint(java.net.URI endpoint, String subscriptionKey) {
        return new SpeechFactory(endpoint, subscriptionKey);
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
      * Gets the service endpoint.
      * @return the service endpoint.
      */
    public URI getEndpoint() {
        return URI.create(_Parameters.getString(FactoryParameterNames.Endpoint));
    }

    /**
      * The collection of parameters and their values defined for this RecognizerFactory.
      * @return The collection of parameters and their values defined for this RecognizerFactory.
      */
    public ParameterCollection<SpeechFactory> getParameters() {
        return _Parameters;
    } // { get; private set; }
    private final ParameterCollection<SpeechFactory> _Parameters;

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer() {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer(), null);
    }

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String language) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer(language), null);
    }

    /**
      * Creates a speech recognizer, using the default microphone input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @param format Output format, simple or detailed.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String language, SpeechOutputFormat format) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer(language,
            format == SpeechOutputFormat.Simple ?
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), null);
    }

    /**
      * Creates a speech recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile), null);
    }

    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile, String language) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile, language), null);
    }
    /**
     * Creates a speech recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithFileInput(String audioFile, String language, SpeechOutputFormat format) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile, language,
                    format == SpeechOutputFormat.Simple ?
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                        com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), null);
    }

    /**
      * Creates a speech recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return A speech recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithStreamImpl(audioStream), audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream, String language) {
       return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithStreamImpl(audioStream, language), audioStream);
    }

    /**
     * Creates a speech recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @param format. Output format, simple or detailed.
     * @return A speech recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizerWithStream(AudioInputStream audioStream, String language, SpeechOutputFormat format) {
       return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithStreamImpl(audioStream, language,
                   format == SpeechOutputFormat.Simple ?
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Simple :
                    com.microsoft.cognitiveservices.speech.internal.OutputFormat.Detailed
                    ), audioStream);
    }
    
    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer() {
        return new IntentRecognizer(factoryImpl.createIntentRecognizer(), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer(String language) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizer(language), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerWithFileInput(String audioFile) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithFileInput(audioFile), null);
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizerWithFileInput(String audioFile, String language) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithFileInput(audioFile, language), null);
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizerWithStream(AudioInputStream audioStream) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithStreamImpl(audioStream), audioStream);
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizerWithStream(AudioInputStream audioStream, String language) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithStreamImpl(audioStream, language), audioStream);
    }

    /**
      * Creates a translation recognizer, using the default microphone input.
      * @param sourceLanguage The spoken language that needs to be translated.
      * @param targetLanguages The language of translation.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizer(String sourceLanguage, ArrayList<String> targetLanguages) {
        com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
        
        for(String element : targetLanguages) {
            v.add(element);
        }

        return new TranslationRecognizer(factoryImpl.createTranslationRecognizer(sourceLanguage, v), null);
    }

    /**
     * Creates a translation recognizer, using the default microphone input.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The language of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizer(String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizer(sourceLanguage, v, voice), null);
   }
    
   /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v), null);
   }

    /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer createTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v, voice), null);
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer createTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithStreamImpl(audioStream, sourceLanguage, v), audioStream);
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer createTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithStreamImpl(audioStream, sourceLanguage, v, voice), audioStream);
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
