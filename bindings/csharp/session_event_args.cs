//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon
{
    /// <summary>
    /// Defines payload for session events like SessionStarted/Stopped, SoundStarted/Stopped.
    /// </summary>
    public class SessionEventArgs : System.EventArgs
    {
        internal SessionEventArgs(Internal.SessionEventArgs arg)
        {
            this.SessionId = arg.SessionId;
        }

        /// <summary>
        /// Specified the seesion identifier.
        /// </summary>
        public string SessionId { get; }
    }
}
