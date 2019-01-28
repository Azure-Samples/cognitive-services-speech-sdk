//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXRECOHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal class SpeechRecognizer : Recognizer
    {
        internal SpeechRecognizer(IntPtr recoPtr) : base(recoPtr)
        {
        }

        public static SpeechRecognizer FromConfig(SpeechConfig speechConfig, AudioConfig audioConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_speech_recognizer_from_config(out rHandle, speechConfig.configHandle, audioConfig != null ? audioConfig.configHandle: IntPtr.Zero));
            GC.KeepAlive(speechConfig);
            GC.KeepAlive(audioConfig);
            return new SpeechRecognizer(rHandle);
        }

        public static SpeechRecognizer FromConfig(SpeechConfig speechConfig)
        {
            SPXRECOHANDLE rHandle = IntPtr.Zero;
            SPXAUDIOCONFIGHANDLE aHandle = IntPtr.Zero;
            ThrowIfFail(SpxFactory.recognizer_create_speech_recognizer_from_config(out rHandle, speechConfig.configHandle, aHandle));
            GC.KeepAlive(speechConfig);
            return new SpeechRecognizer(rHandle);
        }

        public string EndpointId
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceConnection_EndpointId);
            }
        }

        public string AuthorizationToken
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
            }
            set
            {
                Properties.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        public new SpeechRecognitionResult RecognizeOnce()
        {
            SPXRESULTHANDLE hresult = base.RecognizeOnce();
            return new SpeechRecognitionResult(hresult);
        }
    }
}
