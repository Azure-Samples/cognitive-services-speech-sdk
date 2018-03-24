//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Carbon
{
    /// <summary>
    /// Defines payload for session events like SessionStarted/Stopped, SoundStarted/Stopped.
    /// </summary>
    public class SessionEventArgs : System.EventArgs
    {
        internal SessionEventArgs(SessionEventType type,  Internal.SessionEventArgs arg)
        {
            this.SessionId = arg.SessionId;
            this.EventType = type;
        }

        /// <summary>
        /// Represents the event type.
        /// </summary>
        public SessionEventType EventType { get; }

        /// <summary>
        /// Represents the seesion identifier.
        /// </summary>
        public string SessionId { get; }

        /// <summary>
        /// Returns a string that represents the session event.
        /// </summary>
        /// <returns>A string that represents the session event.</returns>
        public override string ToString()
        {
            return string.Format("EventType: {0} SessionId: {1}.", EventType, SessionId);
        }
    }
}
