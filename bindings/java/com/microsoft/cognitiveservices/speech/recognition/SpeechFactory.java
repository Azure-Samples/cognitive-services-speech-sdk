package com.microsoft.cognitiveservices.speech.recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.ParameterCollection;
import com.microsoft.cognitiveservices.speech.recognition.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.recognition.translation.TranslationRecognizer;

 /**
   * Factory methods to create recognizers.
   */
 public final class SpeechFactory implements Closeable
 {
     // load the native library.
    static
    {
        System.loadLibrary("carbon_java_bindings");
    }

    /**
      * Creates an instance of recognizer factory.
      */
    private SpeechFactory() throws UnsupportedOperationException
    {
        com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromSubscription("illegal-subscription-key");
        Parameters = new ParameterCollection<SpeechFactory>(this);
    }

    /**
      * Creates an instance of recognizer factory with specified subscription key or authentication token and region (optional).
      * @param isSubscription If true, authenticates via the subscriptionKey. If false, authenticates via the authorizationToken.
      * @param subscriptionKeyOrAuthorizationToken The subscription key or authenticationToken based on the isSubscription flag.
      * @param region The region name. Pass null if not used.
      */
    private SpeechFactory(boolean isSubscription, String subscriptionKeyOrAuthorizationToken, String region)
    {
        factoryImpl = isSubscription ? 
            com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromSubscription(subscriptionKeyOrAuthorizationToken) :
            com.microsoft.cognitiveservices.speech.internal.SpeechFactory.fromAuthorizationToken(subscriptionKeyOrAuthorizationToken);

        // connect the native properties with the swig layer.
        Parameters = new ParameterCollection<SpeechFactory>(this);

        if (region != null)
        {
            setRegion(region);
        }
    }

    /**
      * Static instance of SpeechFactory returned by passing subscriptionKey 
      * @param subscriptionKey The subscription key.
      * @param region The region name. Pass null if not used.
      * @return The speech factory
      */
    public static SpeechFactory fromSubscription(String subscriptionKey, String region)
    {
        return new SpeechFactory(true, subscriptionKey, region);
    }    

    /**
      * Static instance of SpeechFactory returned by passing authorization token.
      * @param authorizationToken The authorization token.
      * @param region The region name. Pass null if not used.
      * @return The speech factory
      */
    public static SpeechFactory fromAuthentication(String authorizationToken, String region)
    {
        return new SpeechFactory(false, authorizationToken, region);
    }  
    
    /**
      * Gets the subscription key.
      * @return the subscription key.
      */
    public String getSubscriptionKey()
    {
        return Parameters.getString(ParameterNames.SpeechSubscriptionKey);
    }

    /**
      * Gets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @return Gets the authorization token.
      */
    public String getAuthorizationToken()
    {
        return Parameters.getString(ParameterNames.SpeechAuthToken);
    }

    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setAuthorizationToken(String value)
    {
        Parameters.set(ParameterNames.SpeechAuthToken, value);
    }

    /**
      * Gets the region name of the service to be connected.
      * @return the region name of the service to be connected.
      */
    public String getRegion()
    {
        return Parameters.getString(ParameterNames.Region);
    }

    /**
      * Sets the region name of the service to be connected.
      * @param value the region name of the service to be connected.
      */
    public void setRegion(String value)
    {
        Parameters.set(ParameterNames.Region, value);
    }

    /**
      * Gets the service endpoint.
      * @return the service endpoint.
      */
    public String getEndpoint()
    {
        return Parameters.getString(ParameterNames.SpeechEndpoint);
    }

    /**
      * Sets the service endpoint.
      * @param value the service endpoint.
      */
    public void setEndpoint(String value)
    {
        // factoryImpl.SetSubscriptionKey(value);
        Parameters.set(ParameterNames.SpeechEndpoint, value);
    }

    /**
      * The collection of parameters and their values defined for this RecognizerFactory.
      */
    public final ParameterCollection<SpeechFactory> Parameters;// { get; private set; }

    /**
      * Creates a translation recognizer, using the default microphone input.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer() throws UnsupportedOperationException
    {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizer());
    }

    /**
      * Creates a translation recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(String audioFile) throws UnsupportedOperationException
    {
        return new SpeechRecognizer(factoryImpl.createSpeechRecognizerWithFileInput(audioFile));
    }

    /**
      * Creates a translation recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return A translation recognizer instance.
      */
    public SpeechRecognizer createSpeechRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer()
    {
        return new IntentRecognizer(factoryImpl.createIntentRecognizer());
    }

    /**
      * Creates an intent recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return An intent recognizer instance
      */
    public IntentRecognizer createIntentRecognizer(String audioFile)
    {
        return new IntentRecognizer(factoryImpl.createIntentRecognizerWithFileInput(audioFile));
    }

    /**
      * Creates an intent recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return An intent recognizer instance.
      */
    public IntentRecognizer createIntentRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Creates a translation recognizer, using the default microphone input.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizer() throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Creates a translation recognizer, using the specified file as audio input.
      * @param audioFile Specifies the audio input file.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizer(String audioFile) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Creates a translation recognizer, using the specified input stream as audio input.
      * @param audioStream Specifies the audio input stream.
      * @return A translation recognizer instance.
      */
    public TranslationRecognizer createTranslationRecognizer(AudioInputStream audioStream) throws UnsupportedOperationException
    {
        throw new UnsupportedOperationException();
    }

    /**
      * Dispose of associated resources.
      */
    public void close()
    {
        if (disposed)
        {
            return;
        }

        Parameters.close();
        factoryImpl.delete();
        disposed = true;
    }

    public com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory getFactoryImpl()
    {
        return factoryImpl;
    }


    private com.microsoft.cognitiveservices.speech.internal.ICognitiveServicesSpeechFactory factoryImpl;
    private boolean disposed = false;
}
