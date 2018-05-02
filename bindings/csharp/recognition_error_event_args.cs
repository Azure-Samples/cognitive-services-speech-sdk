//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines payload of RecognitionErrorEvent. 
    /// </summary>
    public sealed class RecognitionErrorEventArgs : System.EventArgs
    {
        internal RecognitionErrorEventArgs(string sessionId, RecognitionStatus status)
        {
            Status = status;
            SessionId = sessionId;
        }

        internal RecognitionErrorEventArgs(string sessionId, Microsoft.CognitiveServices.Speech.Internal.Reason reason)
        {
            Trace.Assert((int)Internal.Reason.Recognized == (int)RecognitionStatus.Recognized);
            Trace.Assert((int)Internal.Reason.IntermediateResult == (int)RecognitionStatus.IntermediateResult);
            Trace.Assert((int)Internal.Reason.NoMatch == (int)RecognitionStatus.NoMatch);
            Trace.Assert((int)Internal.Reason.Canceled == (int)RecognitionStatus.Canceled);
            Trace.Assert((int)Internal.Reason.OtherRecognizer == (int)RecognitionStatus.OtherRecognizer);

            Status = (RecognitionStatus)((int)reason);
            SessionId = sessionId;
        }

        /// <summary>
        /// Specifies the error reason.
        /// </summary>
        public RecognitionStatus Status { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get; }

        /// <summary>
        /// Returns a string that represents the recognition error event.
        /// </summary>
        /// <returns>A string that represents the recognition error event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId: {0} Status: {1}.", SessionId, Status); 
        }
    }
}
