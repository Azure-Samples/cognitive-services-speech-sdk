//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class SpeechRecognitionResult : RecognitionResult
    {
        internal SpeechRecognitionResult(IntPtr resultPtr) : base(resultPtr)
        {
        }
    }
}
