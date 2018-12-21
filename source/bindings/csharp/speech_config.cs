//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Speech configuration.
    /// </summary>
    public class SpeechConfig
    {
        // Should be set only by derived classes of this assembly.
        internal Internal.SpeechConfig configImpl { get; set; }

        internal SpeechConfig(Internal.SpeechConfig impl)
        {
            this.configImpl = impl;
            this.configImpl.SetProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "C#");
        }

        /// <summary>
        /// Creates an instance of speech configuration with specified subscription key and region.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key, can be empty if authorization token is specified later.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromSubscription(string subscriptionKey, string region)
        {
            return new SpeechConfig(Internal.SpeechConfig.FromSubscription(subscriptionKey, region));
        }

        /// <summary>
        /// Creates an instance of the speech config with specified authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by setting the property `AuthorizationToken` on the corresponding
        /// recognizer with a new valid token.
        /// </summary>
        /// <param name="authorizationToken">The authorization token.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            return new SpeechConfig(Internal.SpeechConfig.FromAuthorizationToken(authorizationToken, region));
        }

        /// <summary>
        /// Creates an instance of the speech config with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
        /// For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
        /// the language setting in uri takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech config instance.</returns>
        public static SpeechConfig FromEndpoint(Uri endpoint, string subscriptionKey)
        {
            return new SpeechConfig(Internal.SpeechConfig.FromEndpoint(Uri.EscapeUriString(endpoint.ToString()), subscriptionKey));
        }

        /// <summary>
        /// Subscription key.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return this.configImpl.GetSubscriptionKey();
            }
        }

        /// <summary>
        /// Region.
        /// </summary>
        public string Region
        {
            get
            {
                return this.configImpl.GetRegion();
            }
        }

        /// <summary>
        /// Gets the authorization token.
        /// If authorization token is set, the subscription key is ignored.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by setting the property `AuthorizationToken` on the corresponding
        /// recognizer with a new valid token.
        /// Refresh
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return this.configImpl.GetAuthorizationToken();
            }
        }

        /// <summary>
        /// Specifies the name of spoken language to be recognized in BCP-47 format
        /// </summary>
        public string SpeechRecognitionLanguage
        {
            get
            {
                return this.configImpl.GetSpeechRecognitionLanguage();
            }

            set
            {
                this.configImpl.SetSpeechRecognitionLanguage(value);
            }
        }

        /// <summary>
        /// Output format: simple or detailed.
        /// </summary>
        public OutputFormat OutputFormat
        {
            get
            {
                var val = this.configImpl.GetProperty(Internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse);
                return val == "true" ? OutputFormat.Detailed : OutputFormat.Simple;
            }

            set
            {
                this.configImpl.SetProperty(
                    Internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse,
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
                return this.configImpl.GetEndpointId();
            }

            set
            {
                this.configImpl.SetEndpointId(value);
            }
        }

        /// <summary>
        /// Sets proxy configuration.
        /// Added in version 1.1.0
        /// </summary>
        /// <param name="proxyHostName">The host name of the proxy server.</param>
        /// <param name="proxyPort">The port number of the proxy server.</param>
        /// <param name="proxyUserName">The user name of the proxy server. Use empty string if no user name is needed.</param>
        /// <param name="proxyPassword">The password of the proxy server. Use empty string if no password is needed.</param>
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
            if ((proxyUserName == null) || (proxyPassword == null))
            {
                throw new ArgumentNullException("proxy user name or password is empty.");
            }
            this.configImpl.SetProxy(proxyHostName, (uint)proxyPort, proxyUserName, proxyPassword);
        }

        /// <summary>
        /// Sets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(string name, string value)
        {
            this.configImpl.SetProperty(name, value);
        }

        /// <summary>
        /// Gets the property by name.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(string name)
        {
            return this.configImpl.GetProperty(name);
        }
   }
}
