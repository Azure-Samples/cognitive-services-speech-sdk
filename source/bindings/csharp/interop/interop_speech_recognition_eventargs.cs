//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXRESULTHANDLE = System.IntPtr;

    internal class SpeechRecognitionEventArgs : RecognitionEventArgs
    {
        public SpeechRecognitionEventArgs(IntPtr eventPtr) : base(eventPtr)
        {
        }

        public SpeechRecognitionResult Result
        {
            get
            {
                SPXRESULTHANDLE result;
                ThrowIfFail(SpeechRecognizer.recognizer_recognition_event_get_result(eventHandle, out result));
                return new SpeechRecognitionResult(result);
            }
        }
    }
}
