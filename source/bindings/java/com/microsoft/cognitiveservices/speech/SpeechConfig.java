//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

/**
 * Speech configuration.
 * Changed in version 1.7.0.
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

        // prevent classgc from freeing this class
        speechConfigClass = SpeechConfig.class;
    }

    /*! \cond PROTECTED */

    /**
     * Creates an instance of speech config.
     */
    protected SpeechConfig(long handleValue) {
        Contracts.throwIfNull(handleValue, "handleValue");

        this.speechConfigHandle = new SafeHandle(handleValue, SafeHandleType.SpeechConfig);
        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBag(speechConfigHandle, propHandle));
        this.propertyHandle = new PropertyCollection(propHandle);
        this.propertyHandle.setProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Java");
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
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromSubscription(configRef, subscriptionKey, region));
        return new SpeechConfig(configRef.getValue());
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
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromAuthorizationToken(configRef, authorizationToken, region));
        return new SpeechConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech config with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or parameters.
     * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
     * For example, if the recognition language is defined in URI as query parameter "language=de-DE", and also set by setSpeechRecognitionLanguage("en-US"),
     * the language setting in URI takes precedence, and the effective language is "de-DE".
     * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
     * Note: To use an authorization token with fromEndpoint, please use fromEndpoint(java.net.URI),
     * and then call setAuthorizationToken() on the created SpeechConfig instance.
     * @param endpoint The service endpoint to connect to.
     * @param subscriptionKey The subscription key.
     * @return A speech config instance.
     */
    public static SpeechConfig fromEndpoint(java.net.URI endpoint, String subscriptionKey) {
        Contracts.throwIfNull(endpoint, "endpoint");
        if(subscriptionKey == null) {
            throw new NullPointerException("subscriptionKey");
        }
        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromEndpoint(configRef, endpoint.toString(), subscriptionKey));
        return new SpeechConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech config with specified endpoint.
     * This method is intended only for users who use a non-standard service endpoint or parameters.
     * Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
     * For example, if the recognition language is defined in URI as query parameter "language=de-DE", and also set by setSpeechRecognitionLanguage("en-US"),
     * the language setting in URI takes precedence, and the effective language is "de-DE".
     * Only the parameters that are not specified in the endpoint URI can be set by other APIs.
     * Note: if the endpoint requires a subscription key for authentication, please use fromEndpoint(java.net.URI, String) to pass
     * the subscription key as parameter.
     * To use an authorization token with fromEndpoint, use this method to create a SpeechConfig instance, and then
     * call setAuthorizationToken() on the created SpeechConfig instance.
     * Note: Added in version 1.5.0.
     * @param endpoint The service endpoint to connect to.
     * @return A speech config instance.
     */
    public static SpeechConfig fromEndpoint(java.net.URI endpoint) {
        Contracts.throwIfNull(endpoint, "endpoint");

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromEndpoint(configRef, endpoint.toString(), null));
        return new SpeechConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech config with specified host and subscription key.
     * This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
     * For services with a non-standard resource path or no path at all, use fromEndpoint instead.
     * Note: Query parameters are not allowed in the host URI and must be set by other APIs.
     * Note: To use an authorization token with fromHost, use fromHost(java.net.URI),
     * and then call setAuthorizationToken() on the created SpeechConfig instance.
     * Note: Added in version 1.8.0.
     * @param host The service host to connect to. Format is "protocol://host:port" where ":port" is optional.
     * @param subscriptionKey The subscription key.
     * @return A speech config instance.
     */
    public static SpeechConfig fromHost(java.net.URI host, String subscriptionKey) {
        Contracts.throwIfNull(host, "host");
        if(subscriptionKey == null) {
            throw new NullPointerException("subscriptionKey");
        }

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromHost(configRef, host.toString(), subscriptionKey));
        return new SpeechConfig(configRef.getValue());
    }

    /**
     * Creates an instance of the speech config with specified host.
     * This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
     * For services with a non-standard resource path or no path at all, use fromEndpoint instead.
     * Note: Query parameters are not allowed in the host URI and must be set by other APIs.
     * Note: If the host requires a subscription key for authentication, use fromHost(java.net.URI, String) to pass
     * the subscription key as parameter.
     * To use an authorization token with fromHost, use this method to create a SpeechConfig instance, and then
     * call setAuthorizationToken() on the created SpeechConfig instance.
     * Note: Added in version 1.8.0.
     * @param host The service host to connect to. Format is "protocol://host:port" where ":port" is optional.
     * @return A speech config instance.
     */
    public static SpeechConfig fromHost(java.net.URI host) {
        Contracts.throwIfNull(host, "host");

        IntRef configRef = new IntRef(0);
        Contracts.throwIfFail(fromHost(configRef, host.toString(), null));
        return new SpeechConfig(configRef.getValue());
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

        propertyHandle.setProperty(PropertyId.SpeechServiceAuthorization_Token, value);
    }

    /**
     * Gets the authorization token.
     * @return The authorization token.
     */
    public String getAuthorizationToken() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Sets the speech recognition language
     * @param value the language identifier in BCP-47 format.
     */
    public void setSpeechRecognitionLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_RecoLanguage, value);
    }

    /**
     * Gets the speech recognition language
     * @return Returns the recognition language.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Sets speech recognition output format (simple or detailed).
     * Note: This output format is for speech recognition results, use <see cref="SpeechConfig.setSpeechSynthesisOutputFormat"/> to set synthesized audio output format.
     * @param format The speech recognition output format.
     */
    public void setOutputFormat(OutputFormat format) {
        String value = "false";
        if(format == OutputFormat.Detailed) {
            value = "true";
        }
        propertyHandle.setProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, value);
    }

    /**
     * Gets speech recognition output format (simple or detailed).
     * Note: This output format is for speech recognition results, use <see cref="SpeechConfig.getSpeechSynthesisOutputFormat"/> to get synthesized audio output format.
     * @return Returns the speech recognition output format.
     */
    public OutputFormat getOutputFormat() {
        String result = propertyHandle.getProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse);
        return (result.equals("true") ? OutputFormat.Detailed : OutputFormat.Simple);
    }

    /**
     * Sets the endpoint ID of a customized speech model that is used for speech recognition.
     * @param value the endpoint ID.
     */
    public void setEndpointId(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_EndpointId, value);
    }

    /**
     * Gets the endpoint ID of a customized speech model that is used for speech recognition.
     * @return The endpoint ID.
     */
    public String getEndpointId() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_EndpointId);
    }

    /**
     * Sets the speech synthesis language.
     * Added in version 1.7.0
     * @param value the language identifier in BCP-47 format (e.g. en-US).
     */
    public void setSpeechSynthesisLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_SynthLanguage, value);
    }

    /**
     * Gets the speech synthesis language.
     * Added in version 1.7.0
     * @return Returns the synthesis language.
     */
    public String getSpeechSynthesisLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_SynthLanguage);
    }

    /**
     * Sets the speech synthesis voice name.
     * Added in version 1.7.0
     * @param value The speech synthesis voice name.
     */
    public void setSpeechSynthesisVoiceName(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_SynthVoice, value);
    }

    /**
     * Gets the speech synthesis voice name.
     * Added in version 1.7.0
     * @return Returns the synthesis voice name.
     */
    public String getSpeechSynthesisVoiceName() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_SynthVoice);
    }

    /**
     * Sets the speech synthesis output format.
     * Added in version 1.7.0
     * @param value The synthesis output format ID (e.g. Riff16Khz16BitMonoPcm).
     */
    public void setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat value) {
        Contracts.throwIfFail(setAudioOutputFormat(speechConfigHandle, value.ordinal() + 1)); // Native SpeechSynthesisOutputFormat enum starts at 1!!
    }

    /**
     * Gets the speech synthesis output format.
     * Added in version 1.7.0
     * @return Returns the synthesis output format.
     */
    String getSpeechSynthesisOutputFormat() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_SynthOutputFormat);
    }

    /**
     * Sets proxy configuration
     * Added in version 1.1.0.
     * Note: Proxy functionality is not available on macOS. This function will have no effect on this platform.
     * @param proxyHostName the host name of the proxy server, without the protocol scheme (http://)
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
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_ProxyHostName, proxyHostName);
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_ProxyPort, Integer.toString(proxyPort));
        if (proxyUserName != "") {
            propertyHandle.setProperty(PropertyId.SpeechServiceConnection_ProxyUserName, proxyUserName);
        }
        if (proxyPassword != "") {
            propertyHandle.setProperty(PropertyId.SpeechServiceConnection_ProxyPassword, proxyPassword);
        }
    }

    /**
     * Sets a named property as value.
     * @param name the name of the property.
     * @param value the value.
     */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(name, value);
    }

    /**
     * Sets the property by propertyId.
     * Added in version 1.3.0.
     * @param id PropertyId of the property.
     * @param value The value.
     */
    public void setProperty(PropertyId id, String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(id, value);
    }

    /**
     * Gets a named property as value.
     * @param name the name of the property.
     * @return The value.
     */
    public String getProperty(String name) {
        return propertyHandle.getProperty(name);
    }

    /**
     * Gets the property by propertyId.
     * Added in version 1.3.0.
     * @param id PropertyId of the property.
     * @return The value.
     */
    public String getProperty(PropertyId id) {
        return propertyHandle.getProperty(id);
    }

    /**
     *  Sets a property value that will be passed to service using the specified channel.
     *  Added in version 1.5.0.
     *  @param name the property name.
     *  @param value the property value.
     *  @param channel the channel used to pass the specified property to service.
     */
    public void setServiceProperty(String name, String value, ServicePropertyChannel channel) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNullOrWhitespace(value, "value");
        Contracts.throwIfFail(setServiceProperty(speechConfigHandle, name, value, channel.getValue()));
    }

    /**
     * Sets profanity option.
     * Added in version 1.5.0.
     * @param profanity the property option to set.
     */
    public void setProfanity(ProfanityOption profanity)
    {
        Contracts.throwIfFail(setProfanity(speechConfigHandle, profanity.getValue()));
    }

    /**
     * Enable audio logging in service.
     * Added in version 1.5.0.
     */
    public void enableAudioLogging()
    {
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, "true");
    }

    /**
     * Includes word-level timestamps.
     * Added in version 1.5.0.
     */
    public void requestWordLevelTimestamps()
    {
        propertyHandle.setProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
    }

    /**
     * Enable dictation. Only supported in speech continuous recognition.
     * Added in version 1.5.0.
     */
    public void enableDictation()
    {
        propertyHandle.setProperty(PropertyId.SpeechServiceConnection_RecoMode, "DICTATION");
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }

        if (propertyHandle != null)
        {
            propertyHandle.close();
            propertyHandle = null;
        }
        if (speechConfigHandle != null)
        {
            speechConfigHandle.close();            
            speechConfigHandle = null;
        }
        disposed = true;
    }


    /*! \cond INTERNAL */
    public SafeHandle getImpl() {
        return speechConfigHandle;
    }
    /*! \endcond */

    /*! \cond PROTECTED */
    protected SafeHandle speechConfigHandle = null;
    protected PropertyCollection propertyHandle = null;
    /*! \endcond */
        
    private final static native long fromSubscription(IntRef configHandle, String subscriptionKey, String region);
    private final static native long fromAuthorizationToken(IntRef configHandle, String authorizationToken, String region);
    private final static native long fromEndpoint(IntRef configHandle, String endpoint, String subscriptionKey);
    private final static native long fromHost(IntRef configHandle, String host, String subscriptionKey);
    private final native long getPropertyBag(SafeHandle configHandle, IntRef propHandle);
    private final native long setProfanity(SafeHandle configHandle, int profanity);
    private final native long setServiceProperty(SafeHandle configHandle, String name, String value, int channel);
    private final native long setAudioOutputFormat(SafeHandle configHandle, int value);

    private boolean disposed = false;
}
