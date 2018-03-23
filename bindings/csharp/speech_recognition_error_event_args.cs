//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon.Recognition.Speech
{
    /// <summary>
    /// Defines payload of SpeechRecognitionErrorEvent. 
    /// </summary>
    public class SpeechRecognitionErrorEventArgs : System.EventArgs
    {
        internal SpeechRecognitionErrorEventArgs(Carbon.Internal.SpeechRecognitionEventArgs e)
        {
            var result = new SpeechRecognitionResult(e.Result);
            this.Reason = result.Reason;
            this.SessionId = e.SessionId;
        }

        /// <summary>
        /// Specifies the error reason.
        /// </summary>
        public SpeechRecognitionReason Reason { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get;  }
    }
}
