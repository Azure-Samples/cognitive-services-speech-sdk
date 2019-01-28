//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{

    using SPXHR = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXTRIGGERHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal class IntentRecognizer : Recognizer
    {
        internal IntentRecognizer(IntPtr recoPtr) : base(recoPtr)
        {
        }

        public static IntentRecognizer FromConfig(SpeechConfig speechConfig, AudioConfig audioConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_intent_recognizer_from_config(out rHandle, speechConfig.configHandle, audioConfig != null ? audioConfig.configHandle: IntPtr.Zero));
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);
            return new IntentRecognizer(rHandle);
        }

        public static IntentRecognizer FromConfig(SpeechConfig speechConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            SPXAUDIOCONFIGHANDLE aHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_intent_recognizer_from_config(out rHandle, speechConfig.configHandle, aHandle));
            GC.KeepAlive(speechConfig);
            return new IntentRecognizer(rHandle);
        }

        public void AddIntent(string simplePhrase)
        {
            var trigger = IntentTrigger.From(simplePhrase);
            AddIntent(trigger, simplePhrase);
        }

        public void AddIntent(string simplePhrase, string intentId)
        {
            var trigger = IntentTrigger.From(simplePhrase);
            AddIntent(trigger, intentId);
        }

        public void AddIntent(LanguageUnderstandingModel model, string intentName)
        {
            var trigger = IntentTrigger.From(model, intentName);
            AddIntent(trigger, intentName);
        }

        public void AddIntent(LanguageUnderstandingModel model, string intentName, string intentId)
        {
            var trigger = IntentTrigger.From(model, intentName);
            AddIntent(trigger, intentId);
        }

        public void AddAllIntents(LanguageUnderstandingModel model)
        {
            var trigger = IntentTrigger.From(model);
            AddIntent(trigger, "");
        }

        public void AddAllIntents(LanguageUnderstandingModel model, string intentId)
        {
            var trigger = IntentTrigger.From(model);
            AddIntent(trigger, intentId);
        }

        public void AddIntent(IntentTrigger intentTrigger, string intentId)
        {
            ThrowIfFail(intent_recognizer_add_intent(recoHandle, intentId, intentTrigger.triggerHandle));
            GC.KeepAlive(intentTrigger);
        }

        public string AuthorizationToken
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token, "");
            }
            set
            {
                Properties.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        public new IntentRecognitionResult RecognizeOnce()
        {
            SPXRESULTHANDLE hresult = base.RecognizeOnce();
            return new IntentRecognitionResult(hresult);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]

        public static extern SPXHR intent_recognizer_add_intent(SPXRECOHANDLE hreco,
            [MarshalAs(UnmanagedType.LPStr)] string intentId,
            SPXTRIGGERHANDLE htrigger);
    }
}
