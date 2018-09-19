//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines result of speech recognition.
    /// </summary>
    public class SpeechRecognitionResult : RecognitionResult
    {
        internal SpeechRecognitionResult(Internal.SpeechRecognitionResult result) : base(result)
        {
            this.speechResultImpl = result;
        }

        // Hold the reference
        Internal.SpeechRecognitionResult speechResultImpl;
    }
}
