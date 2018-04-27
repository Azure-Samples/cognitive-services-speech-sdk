//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines payload for recognition events like Speech Start / End Detected
    /// </summary>
    public class RecogntionEventArgs : System.EventArgs
    {
        internal RecogntionEventArgs(RecognitionEventType type, Internal.RecognitionEventArgs arg)
        {
            this.SessionId = arg.SessionId;
            this.EventType = type;
            this.Offset = arg.Offset;
        }

        /// <summary>
        /// Represents the event type.
        /// </summary>
        public RecognitionEventType EventType { get; }

        /// <summary>
        /// Represents the session identifier.
        /// </summary>
        public string SessionId { get; }

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
            return string.Format("EventType: {0} SessionId: {1} Offset: {2}", EventType, SessionId, Offset);
        }
    }
}
