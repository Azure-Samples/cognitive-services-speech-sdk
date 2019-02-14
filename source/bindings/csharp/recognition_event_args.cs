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
    /// Defines payload for recognition events like Speech Start/End Detected
    /// </summary>
    public class RecognitionEventArgs : SessionEventArgs
    {
        internal RecognitionEventArgs(IntPtr eventHandlePtr)
            : base(eventHandlePtr)
        {
            ThrowIfNull(eventHandle);
            ulong offset = 0;
            ThrowIfFail(Internal.Recognizer.recognizer_recognition_event_get_offset(eventHandle, ref offset));
            Offset = offset;
        }

        /// <summary>
        /// Represents the message offset in tick (100 nanoseconds)
        /// </summary>
        public ulong Offset { get; }

        /// <summary>
        /// Returns a string that represents the recognition event payload.
        /// </summary>
        /// <returns>A string that represents the recognition event payload.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId: {0} Offset: {1}", SessionId, Offset);
        }

    }
}
