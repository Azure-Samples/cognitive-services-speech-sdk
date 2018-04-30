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
      * Creates an instance of recognizer factory with specified subscription key or authentication token and region (optional).
      * @param isSubscription If true, authenticates via the subscriptionKey. If false, authenticates via the authorizationToken.
      * @param subscriptionKeyOrAuthorizationToken The subscription key or authenticationToken based on the isSubscription flag.
      * @param region The region name. Pass null if not used.
      */
    private SpeechFactory(boolean isSubscription, String subscriptionKeyOrAuthorizationToken, String region) {
        factoryImpl = isSubscription ? 
            com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromSubscription(subscriptionKeyOrAuthorizationToken, region) :
            com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromAuthorizationToken(subscriptionKeyOrAuthorizationToken, region);

        // connect the native properties with the swig layer.
            _Parameters = new ParameterCollection<SpeechFactory>(this);

        if (region != null) {
            setRegion(region);
        }
    }

    /**
      * Static instance of SpeechFactory returned by passing subscriptionKey 
      * @param subscriptionKey The subscription key.
      * @param region The region name. Pass null if not used.
      * @return The speech factory
      */
    public static SpeechFactory fromSubscription(String subscriptionKey, String region) {
        return new SpeechFactory(true, subscriptionKey, region);
    }    

    /**
      * Static instance of SpeechFactory returned by passing authorization token.
      * @param authorizationToken The authorization token.
      * @param region The region name. Pass null if not used.
      * @return The speech factory
      */
    public static SpeechFactory fromAuthorizationToken(String authorizationToken, String region) {
        return new SpeechFactory(false, authorizationToken, region);
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
      * Sets the region name of the service to be connected.
      * @param value the region name of the service to be connected.
      */
    public void setRegion(String value) {
        _Parameters.set(FactoryParameterNames.Region, value);
    }

    /**
      * Gets the service endpoint.
      * @return the service endpoint.
      */
    public URI getEndpoint() {
        return URI.create(_Parameters.getString(FactoryParameterNames.Endpoint));
    }

    /**
      * Sets the service endpoint.
      * @param value the service endpoint.
      */
    public void setEndpoint(URI value) {
        _Parameters.set(FactoryParameterNames.Endpoint, value.toString());
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
      * Creates a translation recognizer, using the default microphone input.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer() {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer());
    }

    /**
      * Creates a translation recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String audioFile) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile));
    }

    /**
     * Creates a translation recognizer, using the specified file as audio input.
     * @param audioFile Specifies the audio input file.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A translation recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizer(String audioFile, String language) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile, language));
    }

    /**
      * Creates a translation recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(AudioInputStream audioStream) {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithStream(audioStream));
    }

    /**
     * Creates a translation recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return A translation recognizer instance.
     */
    public SpeechRecognizer createSpeechRecognizer(AudioInputStream audioStream, String language) {
       return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithStream(audioStream, language));
    }
    
    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer() {
        return new IntentRecognizer(factoryImpl.createIntentRecognizer());
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizer(String audioFile) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithFileInput(audioFile));
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer(AudioInputStream audioStream) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithStream(audioStream));
    }

    /**
     * Creates an intent recognizer, using the specified input stream as audio input.
     * @param audioStream Specifies the audio input stream.
     * @param language Specifies the name of spoken language to be recognized in BCP-47 format.
     * @return An intent recognizer instance.
     */
    public IntentRecognizer createIntentRecognizer(AudioInputStream audioStream, String language) {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithStream(audioStream, language));
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

        return new TranslationRecognizer(factoryImpl.createTranslationRecognizer(sourceLanguage, v));
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

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizer(sourceLanguage, v, voice));
   }
    
   /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer CreateTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v));
   }

    /**
     * Creates a translation recognizer using the specified file as audio input.
     * @param audioFile Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.
     * @param sourceLanguage The spoken language that needs to be translated.
     * @param targetLanguages The target languages of translation.
     * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
     * @return A translation recognizer instance.
     */
   public TranslationRecognizer CreateTranslationRecognizerWithFileInput(String audioFile, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithFileInput(audioFile, sourceLanguage, v, voice));
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithStream(audioStream, sourceLanguage, v));
   }

   /**
    * Creates a translation recognizer using the specified input stream as audio input.
    * @param audioStream Specifies the audio input stream.
    * @param sourceLanguage The spoken language that needs to be translated.
    * @param targetLanguages The target languages of translation.
    * @param voice Specifies the name of voice tag if a synthesized audio output is desired.
    * @return A translation recognizer instance.
    */
   public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, String sourceLanguage, ArrayList<String> targetLanguages, String voice) {
       com.microsoft.cognitiveservices.speech.internal.WstringVector v = new com.microsoft.cognitiveservices.speech.internal.WstringVector();
       
       for(String element : targetLanguages) {
           v.add(element);
       }

       return new TranslationRecognizer(factoryImpl.createTranslationRecognizerWithStream(audioStream, sourceLanguage, v, voice));
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
