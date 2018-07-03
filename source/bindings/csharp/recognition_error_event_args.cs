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
        internal RecognitionErrorEventArgs(string sessionId, RecognitionStatus status, string failureReason)
        {
            Status = status;
            SessionId = sessionId;
            FailureReason = failureReason;
        }

        internal RecognitionErrorEventArgs(string sessionId, Microsoft.CognitiveServices.Speech.Internal.Reason reason, string failureReason)
        {
            Status = (RecognitionStatus)((int)reason);
            SessionId = sessionId;
            FailureReason = failureReason;
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
        /// Failure reason.
        /// </summary>
        public string FailureReason { get; private set; }

        /// <summary>
        /// Returns a string that represents the recognition error event.
        /// </summary>
        /// <returns>A string that represents the recognition error event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "SessionId: '{0}', Status: '{1}', FailureReason: '{2}'.", SessionId, Status, this.FailureReason); 
        }
    }
}
