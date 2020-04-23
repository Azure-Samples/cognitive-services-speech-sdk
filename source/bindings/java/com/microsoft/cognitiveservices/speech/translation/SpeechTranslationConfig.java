//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import java.io.Closeable;
import java.util.ArrayList;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

/**
 * Speech translation configuration.
 */
public final class SpeechTranslationConfig extends SpeechConfig implements Closeable {

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /**
     * Creates an instance of recognizer config.
     */
    private SpeechTranslationConfig(long handleValue) {
        super(handleValue);
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
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromSubscription(configRef, subscriptionKey, region));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Static instance of SpeechTranslationConfig returned by passing authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expipres, the caller needs to refresh it by calling this setter with a new valid token.
     * Otherwise, all the recognizers created by this SpeechTranslationConfig instance will encounter errors during recognition.
     * @param authorizationToken The authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The speech config
     */
    public static SpeechTranslationConfig fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromAuthorizationToken(configRef, authorizationToken, region));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech translation config with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or parameters.
     * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
     * For example, if the recognition language is defined in URI as query parameter "language=de-DE", and also set by setSpeechRecognitionLanguage("en-US"),
     * the language setting in URI takes precedence, and the effective language is "de-DE".
     * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
     * Note: To use an authorization token with fromEndpoint, please use fromEndpoint(java.net.URI),
     * and then call setAuthorizationToken() on the created SpeechTranslationConfig instance.
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @return A SpeechTranslationConfig instance.
     */
    public static SpeechTranslationConfig fromEndpoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        if(subscriptionKey == null) {
            throw new NullPointerException("subscriptionKey");
        }
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromEndpoint(configRef, endpoint.toString(), subscriptionKey));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech translation config with specified endpoint.
     * This method is intended only for users who use a non-standard service endpoint or parameters.
     * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
     * For example, if the recognition language is defined in URI as query parameter "language=de-DE", and also set by setSpeechRecognitionLanguage("en-US"),
     * the language setting in URI takes precedence, and the effective language is "de-DE".
     * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
     * Note: if the endpoint requires a subscription key for authentication, please use fromEndpoint(java.net.URI, String) to pass
     * the subscription key as parameter.
     * To use an authorization token with fromEndpoint, please use this method to create a SpeechTranslationConfig instance, and then
     * call setAuthorizationToken() on the created SpeechTranslationConfig instance.
     * Note: Added in version 1.5.0.
     * @param endpoint The service endpoint to connect to.
     * @return A SpeechTranslationConfig instance.
     */
    public static SpeechTranslationConfig fromEndpoint(java.net.URI endpoint) {
        Contracts.throwIfNull(endpoint, "endpoint");

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromEndpoint(configRef, endpoint.toString(), null));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech translation config with specified host and subscription key.
     * This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
     * For services with a non-standard resource path or no path at all, use fromEndpoint instead.
     * Note: Query parameters are not allowed in the host URI and must be set by other APIs.
     * Note: To use an authorization token with fromHost, use fromHost(java.net.URI),
     * and then call setAuthorizationToken() on the created SpeechTranslationConfig instance.
     * Note: Added in version 1.8.0.
     * @param host The service host to connect to. Format is "protocol://host:port" where ":port" is optional.
     * @param subscriptionKey The subscription key.
     * @return A SpeechTranslationConfig instance.
     */
    public static SpeechTranslationConfig fromHost(java.net.URI host, String subscriptionKey) {
        Contracts.throwIfNull(host, "host");
        if(subscriptionKey == null) {
            throw new NullPointerException("subscriptionKey");
        }

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromHost(configRef, host.toString(), subscriptionKey));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech translation config with specified host.
     * This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
     * For services with a non-standard resource path or no path at all, use fromEndpoint instead.
     * Note: Query parameters are not allowed in the host URI and must be set by other APIs.
     * Note: If the host requires a subscription key for authentication, use fromHost(java.net.URI, String) to pass
     * the subscription key as parameter.
     * To use an authorization token with fromHost, use this method to create a SpeechTranslationConfig instance, and then
     * call setAuthorizationToken() on the created SpeechTranslationConfig instance.
     * Note: Added in version 1.8.0.
     * @param host The service host to connect to. Format is "protocol://host:port" where ":port" is optional.
     * @return A SpeechTranslationConfig instance.
     */
    public static SpeechTranslationConfig fromHost(java.net.URI host) {
        Contracts.throwIfNull(host, "host");

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromHost(configRef, host.toString(), null));
        return new SpeechTranslationConfig(configRef.getValue());
    }

    /**
     * Adds a (text) target language for translation.
     * @param value the language identifier in BCP-47 format.
     */
    public void addTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        Contracts.throwIfFail(addTargetLanguage(speechConfigHandle, value));
    }

    /**
     * Removes a (text) target language for translation.
     * Added in version 1.7.0.
     * @param value the language identifier in BCP-47 format.
     */
    public void removeTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        Contracts.throwIfFail(removeTargetLanguage(speechConfigHandle, value));
    }

    /**
     * Gets all target languages that have been configured for translation.
     * @return the list of target languages.
     */
    public ArrayList<String> getTargetLanguages() {

        ArrayList<String> result = new ArrayList<String>();
        String plainStr = propertyHandle.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        String[] values = plainStr.split(",");
        for (int i = 0; i < values.length; ++i)
        {
            result.add(values[i]);
        }

        return result;
    }

    /**
     * Returns the selected voice name.
     * @return The voice name.
     */
    public String getVoiceName() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
    }

    /**
     * Sets voice of the translated language, enable voice synthesis output.
     * @param value the voice name of synthesis output
     */
    public void setVoiceName(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_TranslationVoice, value);
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }

        super.close();
        disposed = true;
    }

    /*! \cond INTERNAL */

    /**
     * Returns the speech translation config implementation
     * @return The implementation of the speech translation config.
     */
    @Override
    public SafeHandle getImpl() {
        return super.getImpl();
    }

    /*! \endcond */

    private final static native long fromSubscription(IntRef configHandle, String subscriptionKey, String region);
    private final static native long fromAuthorizationToken(IntRef configHandle, String authorizationToken, String region);
    private final static native long fromEndpoint(IntRef configHandle, String endpoint, String subscriptionKey);
    private final static native long fromHost(IntRef configHandle, String host, String subscriptionKey);
    private final native long addTargetLanguage(SafeHandle configHandle, String value);
    private final native long removeTargetLanguage(SafeHandle configHandle, String value);

    private boolean disposed = false;
}
