//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Speech configuration.
    /// </summary>
    public class SpeechConfig
    {
        // Should be set only by derived classes of this assembly.
        internal InteropSafeHandle configHandle;
        internal PropertyCollection progBag = null;

        internal SpeechConfig(IntPtr configPtr)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.SpeechConfig.speech_config_release);
            IntPtr hpropbag = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_get_property_bag(configHandle, out hpropbag));
            progBag = new PropertyCollection(hpropbag);

            SetProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "C#");
        }

        /// <summary>
        /// Creates an instance of speech configuration with specified subscription key and region.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key, can be empty if authorization token is specified later.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromSubscription(string subscriptionKey, string region)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_subscription(out speechConfigHandle, subscriptionKey, region));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the speech config with specified authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
        /// For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
        /// to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
        /// </summary>
        /// <param name="authorizationToken">The authorization token.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_authorization_token(out speechConfigHandle, authorizationToken, region));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the speech config with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
        /// For example, if the recognition language is defined in URI as query parameter "language=de-DE", and the property SpeechRecognitionLanguage is set to "en-US",
        /// the language setting in URI takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URI can be set by other APIs.
        /// Note: To use an authorization token with FromEndpoint, use FromEndpoint(System.Uri),
        /// and then set the AuthorizationToken property on the created SpeechConfig instance.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromEndpoint(Uri endpoint, string subscriptionKey)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_endpoint(out speechConfigHandle, Uri.EscapeUriString(endpoint.ToString()), subscriptionKey));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the speech config with specified endpoint.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
        /// For example, if the recognition language is defined in URI as query parameter "language=de-DE", and the property SpeechRecognitionLanguage is set to "en-US",
        /// the language setting in URI takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URI can be set by other APIs.
        /// Note: If the endpoint requires a subscription key for authentication, use FromEndpoint(System.Uri, string) to pass
        /// the subscription key as parameter.
        /// To use an authorization token with FromEndpoint, use this method to create a SpeechConfig instance, and then
        /// set the AuthorizationToken property on the created SpeechConfig instance.
        /// Note: Added in version 1.5.0.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromEndpoint(Uri endpoint)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_endpoint(out speechConfigHandle, Uri.EscapeUriString(endpoint.ToString()), null));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the speech config with specified host and subscription key.
        /// This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
        /// For services with a non-standard resource path or no path at all, use FromEndpoint instead.
        /// Note: Query parameters are not allowed in the host URI and must be set by other APIs.
        /// Note: To use an authorization token with FromHost, use FromHost(System.Uri),
        /// and then set the AuthorizationToken property on the created SpeechConfig instance.
        /// Note: Added in version 1.8.0.
        /// </summary>
        /// <param name="host">The service host to connect to. Format is "protocol://host:port" where ":port" is optional.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromHost(Uri host, string subscriptionKey)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_host(out speechConfigHandle, Uri.EscapeUriString(host.ToString()), subscriptionKey));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the speech config with specified host.
        /// This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
        /// For services with a non-standard resource path or no path at all, use FromEndpoint instead.
        /// Note: Query parameters are not allowed in the host URI and must be set by other APIs.
        /// Note: If the host requires a subscription key for authentication, use FromHost(System.Uri, string) to pass
        /// the subscription key as parameter.
        /// To use an authorization token with FromHost, use this method to create a SpeechConfig instance, and then
        /// set the AuthorizationToken property on the created SpeechConfig instance.
        /// Note: Added in version 1.8.0.
        /// </summary>
        /// <param name="host">The service host to connect to. Format is "protocol://host:port" where ":port" is optional.</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromHost(Uri host)
        {
            IntPtr speechConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechConfig.speech_config_from_host(out speechConfigHandle, Uri.EscapeUriString(host.ToString()), null));
            return new SpeechConfig(speechConfigHandle);
        }

        /// <summary>
        /// Subscription key.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_Key);
            }
        }

        /// <summary>
        /// Region.
        /// </summary>
        public string Region
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_Region);
            }
        }

        /// <summary>
        /// Gets/sets the authorization token.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
        /// For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
        /// to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
        /// Changed in version 1.3.0.
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }
                progBag.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        /// <summary>
        /// Specifies the name of spoken language to be recognized in BCP-47 format
        /// </summary>
        public string SpeechRecognitionLanguage
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
            }

            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_RecoLanguage, value);
            }
        }

        /// <summary>
        /// Output format: simple or detailed.
        /// </summary>
        public OutputFormat OutputFormat
        {
            get
            {
                var val = progBag.GetProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse);
                return val == "true" ? OutputFormat.Detailed : OutputFormat.Simple;
            }

            set
            {
                progBag.SetProperty(
                    PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse,
                    value == OutputFormat.Detailed ? "true" : "false");
            }
        }

        /// <summary>
        /// Sets the endpoint ID of a customized speech model that is used for speech recognition.
        /// </summary>
        public string EndpointId
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_EndpointId);
            }

            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_EndpointId, value);
            }
        }

        /// <summary>
        /// Gets/sets the speech synthesis language, e.g. en-US
        /// Added in version 1.4.0
        /// </summary>
        public string SpeechSynthesisLanguage
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_SynthLanguage);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_SynthLanguage, value);
            }
        }

        /// <summary>
        /// Gets/sets the speech synthesis voice name
        /// Added in version 1.4.0
        /// </summary>
        public string SpeechSynthesisVoiceName
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_SynthVoice);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_SynthVoice, value);
            }
        }

        /// <summary>
        /// Gets the speech synthesis output format
        /// Added in version 1.4.0
        /// </summary>
        public string SpeechSynthesisOutputFormat
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_SynthOutputFormat);
            }
        }

        /// <summary>
        /// Sets the speech synthesis output format.
        /// Added in version 1.4.0
        /// </summary>
        /// <param name="format">The synthesis output format ID (e.g. Riff16Khz16BitMonoPcm).</param>
        public void SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat format)
        {
            ThrowIfFail(Internal.SpeechConfig.speech_config_set_audio_output_format(configHandle, format));
        }

        /// <summary>
        /// Sets proxy configuration.
        /// Added in version 1.1.0
        ///
        /// Note: Proxy functionality is not available on macOS. This function will have no effect on this platform.
        /// </summary>
        /// <param name="proxyHostName">The host name of the proxy server, without the protocol scheme (http://)</param>
        /// <param name="proxyPort">The port number of the proxy server.</param>
        /// <param name="proxyUserName">The user name of the proxy server.</param>
        /// <param name="proxyPassword">The password of the proxy server.</param>
        public void SetProxy(string proxyHostName, int proxyPort, string proxyUserName, string proxyPassword)
        {
            if (string.IsNullOrWhiteSpace(proxyHostName))
            {
                throw new ArgumentException("Invalid proxy host name.");
            }
            if (proxyPort <= 0)
            {
                throw new ArgumentException("Invalid proxy port.");
            }
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyHostName, proxyHostName);
            progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyPort, proxyPort.ToString(CultureInfo.CurrentCulture));
            if (!String.IsNullOrEmpty(proxyUserName))
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyUserName, proxyUserName);
            }
            if (!String.IsNullOrEmpty(proxyPassword))
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_ProxyPassword, proxyPassword);
            }
        }

        /// <summary>
        /// Sets proxy configuration.
        /// Added in version 1.3.0
        /// </summary>
        /// <param name="proxyHostName">The host name of the proxy server.</param>
        /// <param name="proxyPort">The port number of the proxy server.</param>
        public void SetProxy(string proxyHostName, int proxyPort)
        {
            this.SetProxy(proxyHostName, proxyPort, null, null);
        }

        /// <summary>
        /// Sets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(string name, string value)
        {
            progBag.SetProperty(name, value);
        }

        /// <summary>
        /// Sets the property by propertyId
        /// Added in version 1.3.0.
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(PropertyId id, string value)
        {
            if (String.IsNullOrWhiteSpace(value))
            {
                throw new ArgumentException("Invalid property value.");
            }
            progBag.SetProperty(id, value);
        }

        /// <summary>
        /// Gets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(string name)
        {
            return progBag.GetProperty(name);
        }

        /// <summary>
        /// Gets the property by propertyId
        /// Added in version 1.3.0.
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(PropertyId id)
        {
            return progBag.GetProperty(id);
        }

        /// <summary>
        /// Sets a property value that will be passed to service using the specified channel.
        /// Added in version 1.5.0.
        /// </summary>
        /// <param name="name">The property name.</param>
        /// <param name="value">The property value.</param>
        /// <param name="channel">The channel used to pass the specified property to service.</param>
        public void SetServiceProperty(string name, string value, ServicePropertyChannel channel)
        {
            ThrowIfFail(Internal.SpeechConfig.speech_config_set_service_property(configHandle, name, value, channel));
        }

        /// <summary>
        /// Sets profanity option.
        /// Added in version 1.5.0.
        /// </summary>
        /// <param name="profanity">The property option to set.</param>
        public void SetProfanity(ProfanityOption profanity)
        {
            ThrowIfFail(Internal.SpeechConfig.speech_config_set_profanity(configHandle, profanity));
        }

        /// <summary>
        /// Enable audio logging in service.
        /// Added in version 1.5.0.
        /// </summary>
        public void EnableAudioLogging()
        {
            progBag.SetProperty(PropertyId.SpeechServiceConnection_EnableAudioLogging, "true");
        }

        /// <summary>
        /// Includes word-level timestamps.
        /// Added in version 1.5.0.
        /// </summary>
        public void RequestWordLevelTimestamps()
        {
            progBag.SetProperty(PropertyId.SpeechServiceResponse_RequestWordLevelTimestamps, "true");
        }

        /// <summary>
        /// Enable dictation. Only supported in speech continuous recognition.
        /// Added in version 1.5.0.
        /// </summary>
        public void EnableDictation()
        {
            progBag.SetProperty(PropertyId.SpeechServiceConnection_RecoMode, "DICTATION");
        }
    }
}
