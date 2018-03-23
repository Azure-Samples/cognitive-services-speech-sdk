//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon.Recognition.Speech
{
    /// <summary>
    /// Define payload of speech intermediate/final result events.
    /// </summary>
    public class SpeechRecognitionResultEventArgs : System.EventArgs
    {
        internal SpeechRecognitionResultEventArgs(Carbon.Internal.SpeechRecognitionEventArgs e)
        {
            this.Result = new SpeechRecognitionResult(e.Result);
            this.SessionId = e.SessionId;
        }

        /// <summary>
        /// Specifies the recognition result.
        /// </summary>
        public SpeechRecognitionResult Result { get; }

        /// <summary>
        /// Specifies the session identifier.
        /// </summary>
        public string SessionId { get; }
    }
}
