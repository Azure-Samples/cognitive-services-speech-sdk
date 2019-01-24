//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Speech translation configuration.
    /// </summary>
    public sealed class SpeechTranslationConfig : SpeechConfig
    {
        internal readonly Internal.SpeechTranslationConfig impl;

        internal SpeechTranslationConfig(Internal.SpeechTranslationConfig impl) : base(impl)
        {
            this.impl = impl;
        }

        /// <summary>
        /// Creates an instance of speech translation config with specified subscription key and region.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key, can be empty if authorization token is specified later.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech config instance.</returns>
        public new static SpeechTranslationConfig FromSubscription(string subscriptionKey, string region)
        {
            return new SpeechTranslationConfig(Internal.SpeechTranslationConfig.FromSubscription(subscriptionKey, region));
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
        public new static SpeechTranslationConfig FromAuthorizationToken(string authorizationToken, string region)
        {
            return new SpeechTranslationConfig(Internal.SpeechTranslationConfig.FromAuthorizationToken(authorizationToken, region));
        }

        /// <summary>
        /// Creates an instance of the speech translation config with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint or parameters.
        /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
        /// For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
        /// the language setting in uri takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech config instance.</returns>
        public new static SpeechTranslationConfig FromEndpoint(Uri endpoint, string subscriptionKey)
        {
            return new SpeechTranslationConfig(Internal.SpeechTranslationConfig.FromEndpoint(Uri.EscapeUriString(endpoint.ToString()), subscriptionKey));
        }

        /// <summary>
        /// Gets a collection of languages to translate to.
        /// </summary>
        public ReadOnlyCollection<string> TargetLanguages
        {
            get
            {
                var result = new List<string>();
                var v  = this.impl.GetTargetLanguages();
                for (int i = 0; i < v.Count; ++i)
                {
                    result.Add(v[i]);
                }

                return new ReadOnlyCollection<string>(result);
            }
        }

        /// <summary>
        /// Add a target languages of translation.
        /// In case when speech synthesis is used and several target languages are specified for translation,
        /// the speech will be synthesized only for the first language.
        /// </summary>
        /// <param name="language"></param>
        public void AddTargetLanguage(string language)
        {
            this.impl.AddTargetLanguage(language);
        }

        /// <summary>
        /// Specifies the name of voice tag if a synthesized audio output is desired.
        /// </summary>
        public string VoiceName
        {
            set
            {
                this.impl.SetVoiceName(value);
            }
            get
            {
                return this.impl.GetVoiceName();
            }
        }
    }
}
