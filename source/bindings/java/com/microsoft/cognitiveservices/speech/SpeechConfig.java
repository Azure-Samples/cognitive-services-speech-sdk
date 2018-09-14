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

import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;
import com.microsoft.cognitiveservices.speech.util.Contracts;

 /**
   * Speech configuration.
   */
 public final class SpeechConfig implements Closeable {

    // load the native library. Hold the class active so the
    // class GC does not reclaim it (and the local variables!)
    static Class<?> speechConfigClass = null;
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
        speechConfigClass = SpeechConfig.class;
    }

    /**
      * Creates an instance of speech config.
      */
    private SpeechConfig(com.microsoft.cognitiveservices.speech.internal.SpeechConfig configImpl) {
        Contracts.throwIfNull(configImpl, "configImpl");

        this.speechConfigImpl = configImpl;
    }

    /**
      * Creates an instance of a speech config with specified subscription key and service region.
      * @param subscriptionKey The subscription key.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return A speech config instance.
      */
    public static SpeechConfig fromSubscription(String subscriptionKey, String region) {
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechConfig(com.microsoft.cognitiveservices.speech.internal.SpeechConfig.FromSubscription(subscriptionKey, region));
    }

    /**
      * Creates an instance of a speech config with specified authorization token and service region.
      * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
      * expires, the caller needs to refresh it by calling `setAuthorizationToken` with a new valid token.
      * Otherwise, all the recognizers created by this SpeechConfig instance will encounter errors during recognition.
      * For long-living recognizers, `setAuthorizationToken` needs to called on the recognizer.
      * @param authorizationToken The authorization token.
      * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
      * @return A speech config instance.
      */
    public static SpeechConfig fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new SpeechConfig(com.microsoft.cognitiveservices.speech.internal.SpeechConfig.FromAuthorizationToken(authorizationToken, region));
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
    public static SpeechConfig fromEndpoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        Contracts.throwIfIllegalSubscriptionKey(subscriptionKey, "subscriptionKey");

        return new SpeechConfig(com.microsoft.cognitiveservices.speech.internal.SpeechConfig.FromEndpoint(endpoint.toString(), subscriptionKey));
    }

    /**
      * Sets the authorization token.
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setAuthorizationToken(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

       speechConfigImpl.SetAuthorizationToken(value);
    }

    /**
      * Sets the speech recognition language
      * If this is set, subscription key is ignored.
      * User needs to make sure the provided authorization token is valid and not expired.
      * @param value the authorization token.
      */
    public void setSpeechRecognitionLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        speechConfigImpl.SetSpeechRecognitionLanguage(value);        
    }

    /**
      * Sets output format.
      */
    public void setOutputFormat(OutputFormat format) {
        String value = "false";
        if(format == OutputFormat.Detailed) {
            value = "true";
        }
        speechConfigImpl.SetProperty(com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, value);        
    }

    /**
      * Sets the endpoint ID of a customized speech model that is used for speech recognition.
      * @param value the endpoint ID
      */
    public void setEndpointId(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

       speechConfigImpl.SetEndpointId(value);
    }

    /**
      * Sets a named property as value
      * @param name the name of the property
      * @param value the value
      */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        speechConfigImpl.SetProperty(name, value);        
    }

    /**
      * Dispose of associated resources.
      */
    public void close() {
        if (disposed) {
            return;
        }

        speechConfigImpl.delete();
        disposed = true;
    }
    
    /**
      * Returns the Speech Config
      * @return The implementation of the Speech Config
      */
    public com.microsoft.cognitiveservices.speech.internal.SpeechConfig getImpl()
    {
        return speechConfigImpl;
    }
    private com.microsoft.cognitiveservices.speech.internal.SpeechConfig speechConfigImpl;
    private boolean disposed = false;
}
