//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines payload for session events like SessionStarted/Stopped, SoundStarted/Stopped.
    /// </summary>
    public sealed class SessionEventArgs : System.EventArgs
    {
        internal SessionEventArgs(SessionEventType type,  Internal.SessionEventArgs arg)
        {
            eventArgImpl = arg;
            SessionId = arg.SessionId;
            EventType = type;
        }

        /// <summary>
        /// Represents the event type.
        /// </summary>
        public SessionEventType EventType { get; }

        /// <summary>
        /// Represents the session identifier.
        /// </summary>
        public string SessionId { get; }

        /// <summary>
        /// Returns a string that represents the session event.
        /// </summary>
        /// <returns>A string that represents the session event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "EventType: {0} SessionId: {1}.", EventType, SessionId);
        }

        // Hold the reference
        Internal.SessionEventArgs eventArgImpl;
    }
}
