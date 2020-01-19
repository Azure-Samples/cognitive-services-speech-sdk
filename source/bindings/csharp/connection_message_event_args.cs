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
    /// Defines payload for Connection's MessageReceived events
    /// Added in version 1.10.0.
    /// </summary>
    public sealed class ConnectionMessageEventArgs : System.EventArgs
    {
        internal InteropSafeHandle eventHandle;

        internal ConnectionMessageEventArgs(IntPtr eventHandlePtr)
        {
            ThrowIfNull(eventHandlePtr);
            eventHandle = new InteropSafeHandle(eventHandlePtr, Internal.Connection.connection_message_received_event_handle_release);

            IntPtr message = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_message_received_event_get_message(eventHandle, out message));
            Message = new ConnectionMessage(message);
        }

        /// <summary>
        /// Gets the <see cref="ConnectionMessage"/> associated with this <see cref="ConnectionMessageEventArgs"/>.
        /// </summary>
        /// <returns>A ConnectionMessage containing the message.</returns>
        public ConnectionMessage Message { get; }

        /// <summary>
        /// Returns a string that represents the connection message event.
        /// </summary>
        /// <returns>A string that represents the connection message event.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture, "Message: {0}.", Message.ToString());
        }
    }
}
