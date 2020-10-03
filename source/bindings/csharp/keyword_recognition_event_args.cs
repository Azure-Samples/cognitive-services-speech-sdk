//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Class for the events emitted by the <see cref="KeywordRecognizer" />.
    /// </summary>
    public class KeywordRecognitionEventArgs: RecognitionEventArgs
    {
        internal KeywordRecognitionEventArgs(IntPtr eventHandlePtr): base(eventHandlePtr)
        {
            IntPtr result = IntPtr.Zero;
            ThrowIfFail(Internal.Recognizer.recognizer_recognition_event_get_result(eventHandle, out result));
            Result = new KeywordRecognitionResult(result);
        }

        /// <summary>
        /// Keyword recognition event result.
        /// </summary>
        public KeywordRecognitionResult Result { get; }

        /// <summary>
        /// Returns a string that represents the keyword recognition result event.
        /// </summary>
        /// <returns>A string that represents the keyword recognition result event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"SessionId:{0} ResultId:{1} Reason:{2} Recognized text:<{3}>.", SessionId, Result.ResultId, Result.Reason, Result.Text);
        }
    }
}
