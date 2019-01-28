//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXSPEECHCONFIGHANDLE = System.IntPtr;
 
    internal class SpeechTranslationConfig : SpeechConfig
    {
        private string targetLanguages = string.Empty;
        private readonly object configLock = new object();

        internal SpeechTranslationConfig(IntPtr configPtr) : base(configPtr)
        {
        }

        public new static SpeechTranslationConfig FromSubscription(string subscription, string region)
        {
            SPXSPEECHCONFIGHANDLE hconfig = IntPtr.Zero;
            ThrowIfFail(speech_config_from_subscription(out hconfig, subscription, region));
            return new SpeechTranslationConfig(hconfig);
        }

        public new static SpeechTranslationConfig FromAuthorizationToken(string authToken, string region)
        {
            SPXSPEECHCONFIGHANDLE hconfig = IntPtr.Zero;
            ThrowIfFail(speech_config_from_authorization_token(out hconfig, authToken, region));
            return new SpeechTranslationConfig(hconfig);
        }

        public new static SpeechTranslationConfig FromEndpoint(string endpoint, string subscription)
        {
            SPXSPEECHCONFIGHANDLE hconfig = IntPtr.Zero;
            ThrowIfFail(speech_config_from_endpoint(out hconfig, endpoint, subscription));
            return new SpeechTranslationConfig(hconfig);
        }

        public void AddTargetLanguage(string language)
        {
            lock(configLock)
            {
                if (targetLanguages != string.Empty)
                {
                    targetLanguages += ",";
                }
                targetLanguages += language;
            }
            progBag.SetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages, targetLanguages);
        }

        public string[] GetTargetLanguages()
        {
            var targetLanguages = progBag.GetProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
            if (targetLanguages == string.Empty)
            {
                return new string[0];
            }
            return targetLanguages.Split(',');
        }

        public string VoiceName
        {
            get
            {
                return progBag.GetProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
            }
            set
            {
                progBag.SetProperty(PropertyId.SpeechServiceConnection_TranslationFeatures, "textToSpeech");
                progBag.SetProperty(PropertyId.SpeechServiceConnection_TranslationVoice, value);
            }
        }
    }
}
