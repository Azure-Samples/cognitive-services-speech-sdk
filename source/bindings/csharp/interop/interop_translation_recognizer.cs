//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal class TranslationRecognizer : Recognizer
    {
        internal TranslationRecognizer(IntPtr recoPtr) : base(recoPtr)
        {
        }

        public static TranslationRecognizer FromConfig(SpeechTranslationConfig speechConfig, AudioConfig audioConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_translation_recognizer_from_config(out rHandle, speechConfig.configHandle, audioConfig != null ? audioConfig.configHandle : IntPtr.Zero));
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);
            return new TranslationRecognizer(rHandle);
        }

        public static TranslationRecognizer FromConfig(SpeechTranslationConfig speechConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            SPXAUDIOCONFIGHANDLE aHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_translation_recognizer_from_config(out rHandle, speechConfig.configHandle, aHandle));
            GC.KeepAlive(speechConfig);
            return new TranslationRecognizer(rHandle);
        }

        public string AuthorizationToken
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token, string.Empty);
            }
            set
            {
                Properties.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        public new TranslationRecognitionResult RecognizeOnce()
        {
            SPXRESULTHANDLE hresult = base.RecognizeOnce();
            return new TranslationRecognitionResult(hresult);
        }

        public void SetSynthesizingAudioCallback(CallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(translator_synthesizing_audio_set_callback(recoHandle, callbackDelegate, gcHandlePtr));
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR translator_synthesizing_audio_set_callback(SPXRECOHANDLE hreco, CallbackFunctionDelegate pCallback, IntPtr pvContext);

    }
}
