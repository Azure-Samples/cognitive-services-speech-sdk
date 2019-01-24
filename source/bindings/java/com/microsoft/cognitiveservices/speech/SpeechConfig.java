//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Speech configuration.
 */
public class SpeechConfig implements Closeable {

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

            // Note: in case the class exists, we call it, ignoring ANY error it raises.
            //       This is on purpose as ONLY the native loader is responsible for loading
            //       the native binding.
            //       In case the loader does not exist, fall back to depending on the runtime
            //       to locate the library for us. This is e.g. necessary on Android.
            if (ncl != null) {
                try {
                    java.lang.reflect.Method nclm = ncl.getMethod("loadNativeBinding");
                    nclm.invoke(null); // static.
                }
                catch (Exception ex) {
                    // ignored.
                    // in particular, we DON'T want to fallback to the platform
                    // loader here as the native loader has already failed and
                    // we don't want to pick up some random library.
                }
            }
            else {
                // trigger an exception so the handler below calls the
                // default loader.
                throw new NullPointerException("no native loader available");
            }
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

        // setup native tmpdir
        com.microsoft.cognitiveservices.speech.internal.carbon_javaJNI.SetTempDirectory(System.getProperty("java.io.tmpdir"));

        // prevent classgc from freeing this class
        speechConfigClass = SpeechConfig.class;
    }

    /*! \cond PROTECTED */

    /**
     * Creates an instance of speech config.
     */
    protected SpeechConfig(com.microsoft.cognitiveservices.speech.internal.SpeechConfig configImpl) {
        Contracts.throwIfNull(configImpl, "configImpl");

        this.speechConfigImpl = configImpl;
        speechConfigImpl.SetProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Java");
    }

    /*! \endcond */

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
     * expires, the caller needs to refresh it by calling this setter with a new valid token.
     * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
     * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
     * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
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
     * This method is intended only for users who use a non-standard service endpoint or parameters.
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
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expires, the caller needs to refresh it by calling this setter with a new valid token.
     * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
     * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
     * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
     * @param value the authorization token.
     */
    public void setAuthorizationToken(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        speechConfigImpl.SetAuthorizationToken(value);
    }

    /**
     * Gets the authorization token.
     * @return The authorization token.
     */
    public String getAuthorizationToken() {
        return speechConfigImpl.GetAuthorizationToken();
    }

    /**
     * Sets the speech recognition language
     * @param value the language identifier in BCP-47 format.
     */
    public void setSpeechRecognitionLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        speechConfigImpl.SetSpeechRecognitionLanguage(value);
    }

    /**
     * Gets the speech recognition language
     * @return Returns the recognition language.
     */
    public String getSpeechRecognitionLanguage() {
        return speechConfigImpl.GetSpeechRecognitionLanguage();
    }

    /**
     * Sets output format.
     * @param format The output format.
     */
    public void setOutputFormat(OutputFormat format) {
        String value = "false";
        if(format == OutputFormat.Detailed) {
            value = "true";
        }
        speechConfigImpl.SetProperty(com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, value);
    }

    /**
     * Gets output format.
     * @return Returns the output format.
     */
    public OutputFormat getOutputFormat() {
        String result = speechConfigImpl.GetProperty(com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse);
        return (result.equals("true") ? OutputFormat.Detailed : OutputFormat.Simple);
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
     * Gets the endpoint ID of a customized speech model that is used for speech recognition.
     * @return The endpoint ID
     */
    public String getEndpointId() {
        return speechConfigImpl.GetEndpointId();
    }

    /**
     * Sets proxy configuration
     * Added in version 1.1.0
     * @param proxyHostName the host name of the proxy server.
     * @param proxyPort the port number of the proxy server.
     * @param proxyUserName the user name of the proxy server. Use empty string if no user name is needed.
     * @param proxyPassword the password of the proxy server. Use empty string if no user password is needed.
     */
    public void setProxy(String proxyHostName, int proxyPort, String proxyUserName, String proxyPassword) {
        Contracts.throwIfNullOrWhitespace(proxyHostName, "proxyHostName");
        Contracts.throwIfNull(proxyUserName, "proxyUserName");
        Contracts.throwIfNull(proxyPassword, "proxyPassword");
        if (proxyPort <= 0) {
            throw new IllegalArgumentException("invalid proxy port");
        }
        speechConfigImpl.SetProxy(proxyHostName, proxyPort, proxyUserName, proxyPassword);
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
     * Gets a named property as value
     * @param name the name of the property
     * @return The value
     */
    public String getProperty(String name) {
        return speechConfigImpl.GetProperty(name);
    }

    /**
     * Dispose of associated resources.
     */
    @Override
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
