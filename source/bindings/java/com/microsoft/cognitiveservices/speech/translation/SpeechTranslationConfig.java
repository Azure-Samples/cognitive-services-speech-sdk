package com.microsoft.cognitiveservices.speech.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.net.URI;
import java.util.ArrayList;
import java.io.File;
import java.io.IOException;

import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.util.Contracts;

 /**
   * Speech translation configuration.
   */
 public final class SpeechTranslationConfig implements Closeable {

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            // ignored.
        }
    }

    /**
      * Creates an instance of recognizer config.
      */
    private SpeechTranslationConfig(com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig configImpl) {
        Contracts.throwIfNull(configImpl, "configImpl");

        this.translatorConfigImpl = configImpl;
    }

    /**
      * Static instance of SpeechTranslationConfig returned by passing subscriptionKey and service region.
      * @param subscriptionKey The subscription key.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return The speech config 
      */
    public static SpeechTranslationConfig fromSubscription(String subscriptionKey, String region) {
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechTranslationConfig(com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig.FromSubscription(subscriptionKey, region));
    }

    /**
      * Static instance of SpeechTranslationConfig returned by passing authorization token and service region.
      * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
      * expipres, the caller needs to refresh it by setting the property `AuthorizationToken` with a new valid token.
      * Otherwise, all the recognizers created by this SpeechTranslationConfig instance will encounter errors during recognition.
      * @param authorizationToken The authorization token.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return The speech config 
      */
    public static SpeechTranslationConfig fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechTranslationConfig(com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig.FromAuthorizationToken(authorizationToken, region));
    }

    /**
      * Creates an instance of the speech config with specified endpoint and subscription key.
      * This method is intended only for users who use a non-standard service endpoint or paramters.
      * Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
      * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
      * the language setting in uri takes precedence, and the effective language is "de-DE".
      * Only the parameters that are not specified in the endpoint URL can be set by other APIs.
      * @param endpoint The service endpoint to connect to.
      * @param subscriptionKey The subscription key.
      * @return A speech config instance.
      */
    public static SpeechTranslationConfig fromEndpoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");

        return new SpeechTranslationConfig(com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig.FromEndpoint(endpoint.toString(), subscriptionKey));
    }

    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setAuthorizationToken(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        translatorConfigImpl.SetAuthorizationToken(value);
    }
    
    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setSpeechRecognitionLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        translatorConfigImpl.SetSpeechRecognitionLanguage(value);        
    }
    
    /**
      * Adds a (text) target language for translation.
      * @param value the language identifier in BCP-47 format.
      */
    public void addTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        translatorConfigImpl.AddTargetLanguage(value);        
    }


    /**
      * Gets all target languages that have been added for translation.
      * @return the list of target languages.
      */
    public ArrayList<String> getTargetLanguages() {

        ArrayList<String> result = new ArrayList<String>();
        com.microsoft.cognitiveservices.speech.internal.StringVector v = translatorConfigImpl.GetTargetLanguages();
        for (int i = 0; i < v.size(); ++i)
        {
            result.add(v.get(i));
        }

        return result;
    }

    /**
      * Sets voice of the translated language, enable voice synthesis output.
      * @param value the voice name of synthesis output
      */
    public void setVoiceName(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        translatorConfigImpl.SetVoiceName(value);        
    }

    /**
      * Sets a named property as value
      * @param name the name of the property
      * @param value the value
      */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        translatorConfigImpl.SetProperty(name, value);        
    }

    /**
      * Dispose of associated resources.
      */
    public void close() {
        if (disposed) {
            return;
        }

        translatorConfigImpl.delete();
        disposed = true;
    }
    
    /**
     * Returns the speech translation config implementation
     * @return The implementation of the speech translation config.
     */
    com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig getImpl()
    {
        return translatorConfigImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechTranslationConfig translatorConfigImpl;
    private boolean disposed = false;
}
