//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Diagnostics;

namespace Carbon.Recognition
{
    /// <summary>
    /// Defines payload of RecognitionErrorEvent. 
    /// </summary>
    public class RecognitionErrorEventArgs : System.EventArgs
    {
        internal RecognitionErrorEventArgs(string sessionId, RecognitionStatus status)
        {
            this.Status = status;
            this.SessionId = sessionId;
        }

        internal RecognitionErrorEventArgs(string sessionId, Carbon.Internal.Reason reason)
        {
            Debug.Assert((int)Internal.Reason.Recognized == (int)RecognitionStatus.Success);
            Debug.Assert((int)Internal.Reason.IntermediateResult == (int)RecognitionStatus.IntermediateResult);
            Debug.Assert((int)Internal.Reason.NoMatch == (int)RecognitionStatus.NoMatch);
            Debug.Assert((int)Internal.Reason.Canceled == (int)RecognitionStatus.Canceled);
            Debug.Assert((int)Internal.Reason.OtherRecognizer == (int)RecognitionStatus.OtherRecognizer);

            this.Status = (RecognitionStatus)((int)reason);
            this.SessionId = sessionId;
        }

        /// <summary>
        /// Specifies the error reason.
        /// </summary>
        public RecognitionStatus Status { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get;  }

        /// <summary>
        /// Returns a string that represents the recognition error event.
        /// </summary>
        /// <returns>A string that represents the recognition error event.</returns>
        public override string ToString()
        {
            return string.Format("SessionId: {0} Status: {1}.", SessionId, Status); 
        }
    }
}
