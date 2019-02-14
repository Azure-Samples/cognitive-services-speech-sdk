//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines payload for SessionStarted/Stopped events.
    /// </summary>
    public class SessionEventArgs : System.EventArgs
    {
        internal InteropSafeHandle eventHandle;
        private const Int32 maxUUID = 36;
        private const Int32 maxSessionID = maxUUID + 1;

        internal SessionEventArgs(IntPtr eventHandlePtr)
        {
            ThrowIfNull(eventHandlePtr);
            eventHandle = new InteropSafeHandle(eventHandlePtr, Internal.EventArgs.recognizer_event_handle_release);

            SessionId = SpxFactory.GetDataFromHandleUsingDelegate(Internal.Recognizer.recognizer_session_event_get_session_id, eventHandle, maxSessionID);
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
    }
}
