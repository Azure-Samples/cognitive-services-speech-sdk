//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXRESULTHANDLE = System.IntPtr;

    internal class TranslationRecognitionEventArgs : RecognitionEventArgs
    {
        public TranslationRecognitionEventArgs(IntPtr eventPtr) : base(eventPtr)
        {
        }

        public TranslationRecognitionResult Result
        {
            get
            {
                SPXRESULTHANDLE result;
                ThrowIfFail(TranslationRecognizer.recognizer_recognition_event_get_result(eventHandle, out result));
                return new TranslationRecognitionResult(result);
            }
        }
    }
}
