//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Speech translation configuration.
    /// </summary>
    public sealed class SpeechTranslationConfig : SpeechConfig
    {
        private string targetLanguages = string.Empty;

        internal SpeechTranslationConfig(IntPtr configPtr) : base(configPtr)
        {
        }

        /// <summary>
        /// Creates an instance of speech translation config with specified subscription key and region.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key, can be empty if authorization token is specified later.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public new static SpeechTranslationConfig FromSubscription(string subscriptionKey, string region)
        {
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_subscription(out config, subscriptionKey, region));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter on the corresponding
        /// recognizer with a new valid token.
        /// </summary>
        /// <param name="authorizationToken">The authorization token.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public new static SpeechTranslationConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_authorization_token(out config, authorizationToken, region));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
        /// For example, if the recognition language is defined in URI as query parameter "language=de-DE", and the property SpeechRecognitionLanguage is set to "en-US",
        /// the language setting in URI takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URI can be set by other APIs.
        /// Note: To use an authorization token with FromEndpoint, use FromEndpoint(System.Uri),
        /// and then set the AuthorizationToken property on the created SpeechTranslationConfig instance.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A SpeechTranslationConfig instance.</returns>
        public new static SpeechTranslationConfig FromEndpoint(Uri endpoint, string subscriptionKey)
        {
            ThrowIfNull(endpoint);
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_endpoint(out config, Uri.EscapeUriString(endpoint.ToString()), subscriptionKey));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified endpoint.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URI are not changed, even if they are set by any other APIs.
        /// For example, if the recognition language is defined in URI as query parameter "language=de-DE", and the property SpeechRecognitionLanguage is set to "en-US",
        /// the language setting in URI takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URI can be set by other APIs.
        /// Note: If the endpoint requires a subscription key for authentication, use FromEndpoint(System.Uri, string) to pass
        /// the subscription key as parameter.
        /// To use an authorization token with FromEndpoint, please use this method to create a SpeechTranslationConfig instance, and then
        /// set the AuthorizationToken property on the created SpeechTranslationConfig instance.
        /// Note: Added in version 1.5.0.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <returns>A SpeechTranslationConfig instance.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public new static SpeechTranslationConfig FromEndpoint(Uri endpoint)
        {
            ThrowIfNull(endpoint);
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_endpoint(out config, Uri.EscapeUriString(endpoint.ToString()), null));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified host and subscription key.
        /// This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
        /// For services with a non-standard resource path or no path at all, use FromEndpoint instead.
        /// Note: Query parameters are not allowed in the host URI and must be set by other APIs.
        /// Note: To use an authorization token with FromHost, use FromHost(System.Uri),
        /// and then set the AuthorizationToken property on the created SpeechTranslationConfig instance.
        /// Note: Added in version 1.8.0.
        /// </summary>
        /// <param name="host">The service host to connect to. Format is "protocol://host:port" where ":port" is optional.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A SpeechTranslationConfig instance.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public new static SpeechTranslationConfig FromHost(Uri host, string subscriptionKey)
        {
            ThrowIfNull(host);
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_host(out config, Uri.EscapeUriString(host.ToString()), subscriptionKey));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified host.
        /// This method is intended only for users who use a non-default service host. Standard resource path will be assumed.
        /// For services with a non-standard resource path or no path at all, use FromEndpoint instead.
        /// Note: Query parameters are not allowed in the host URI and must be set by other APIs.
        /// Note: If the host requires a subscription key for authentication, use FromHost(System.Uri, string) to pass
        /// the subscription key as parameter.
        /// To use an authorization token with FromHost, please use this method to create a SpeechTranslationConfig instance, and then
        /// set the AuthorizationToken property on the created SpeechTranslationConfig instance.
        /// Note: Added in version 1.8.0.
        /// </summary>
        /// <param name="host">The service host to connect to. Format is "protocol://host:port" where ":port" is optional.</param>
        /// <returns>A SpeechTranslationConfig instance.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public new static SpeechTranslationConfig FromHost(Uri host)
        {
            ThrowIfNull(host);
            IntPtr config = IntPtr.Zero;
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_from_host(out config, Uri.EscapeUriString(host.ToString()), null));
            return new SpeechTranslationConfig(config);
        }

        /// <summary>
        /// Gets a collection of languages to translate to.
        /// </summary>
        public ReadOnlyCollection<string> TargetLanguages
        {
            get
            {
                var plainStr = progBag.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
                return new ReadOnlyCollection<string>(plainStr.Split(','));
            }
        }

        /// <summary>
        /// Adds a target languages of translation.
        /// In case when speech synthesis is used and several target languages are specified for translation,
        /// the speech will be synthesized only for the first language.
        /// </summary>
        /// <param name="language"></param>
        public void AddTargetLanguage(string language)
        {
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_add_target_language(configHandle, language));
        }

        /// <summary>
        /// Removes a target languages of translation.
        /// Added in version 1.7.0.
        /// </summary>
        /// <param name="language"></param>
        public void RemoveTargetLanguage(string language)
        {
            ThrowIfFail(Internal.SpeechTranslationConfig.speech_translation_config_remove_target_language(configHandle, language));
        }

        /// <summary>
        /// Specifies the name of voice tag if a synthesized audio output is desired.
        /// </summary>
        public string VoiceName
        {
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_TranslationVoice, value);
            }
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
            }
        }
    }
}
