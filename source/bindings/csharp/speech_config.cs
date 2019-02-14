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
        /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
        /// For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
        /// the language setting in uri takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
        /// Note: To use authorization token with FromEndpoint, pass an empty string to the subscriptionKey in the FromEndpoint method,
        /// and then set the AuthorizationToken property on the created SpeechConfig instance to use the authorization token.
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
        /// Sets proxy configuration.
        /// Added in version 1.1.0
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
        /// Gets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(string name)
        {
            return progBag.GetProperty(name);
        }
   }
}
