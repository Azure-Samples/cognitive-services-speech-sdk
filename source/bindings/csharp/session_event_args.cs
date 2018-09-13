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
        internal SessionEventArgs(Internal.SessionEventArgs arg)
        {
            eventArgImpl = arg;
            SessionId = arg.SessionId;
        }

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
            return string.Format(CultureInfo.InvariantCulture, "SessionId: {0}.", SessionId);
        }

        // Hold the reference
        Internal.SessionEventArgs eventArgImpl;
    }
}
