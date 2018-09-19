//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines payload for recognition events like Speech Start / End Detected
    /// </summary>
    public class RecognitionEventArgs : SessionEventArgs
    {
        internal RecognitionEventArgs(Internal.RecognitionEventArgs arg)
            : base(arg)
        {
            eventArgsImpl = arg;
            Offset = arg.Offset;
        }

        /// <summary>
        /// Represents the message offset
        /// </summary>
        public ulong Offset { get; }

        /// <summary>
        /// Returns a string that represents the session event.
        /// </summary>
        /// <returns>A string that represents the session event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId: {0} Offset: {1}", SessionId, Offset);
        }

        // hold the reference
        private Internal.RecognitionEventArgs eventArgsImpl;
    }
}
