//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// ConnectionMessage represents implementation specific messages sent to and received from
    /// the speech service. These messages are provided for debugging purposes and should not
    /// be used for production use cases with the Azure Cognitive Services Speech Service.
    /// Messages sent to and received from the Speech Service are subject to change without
    /// notice. This includes message contents, headers, payloads, ordering, etc.
    /// Added in version 1.10.0.
    /// </summary>
    public class ConnectionMessage
    {
        internal ConnectionMessage(IntPtr connectionMessageHandlePtr)
        {
            ThrowIfNull(connectionMessageHandlePtr);
            connectionMessageHandle = new InteropSafeHandle(connectionMessageHandlePtr, Internal.Connection.connection_message_handle_release);

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Connection.connection_message_get_property_bag(connectionMessageHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);

            Path = Properties.GetProperty("connection.message.path");
        }

        /// <summary>
        /// The message path.
        /// </summary>
        public string Path { get; }

        /// <summary>
        /// Checks to see if the ConnectionMessage is a text message.
        /// See also IsBinaryMessage().
        /// </summary>
        /// <returns>A bool indicated if the message payload is text.</returns>
        public bool IsTextMessage()
        {
            ThrowIfNull(connectionMessageHandle);
            return Properties.GetProperty("connection.message.type") == "text";
        }

        /// <summary>
        /// Checks to see if the ConnectionMessage is a binary message.
        /// See also GetBinaryMessage().
        /// </summary>
        /// <returns>A bool indicated if the message payload is binary.</returns>
        public bool IsBinaryMessage()
        {
            ThrowIfNull(connectionMessageHandle);
            return Properties.GetProperty("connection.message.type") == "binary";
        }

        /// <summary>
        /// Gets the text message payload. Typically the text message content-type is
        /// application/json. To determine other content-types use
        /// Properties.GetProperty("Content-Type").
        /// </summary>
        /// <returns>A string containing the text message.</returns>
        public string GetTextMessage()
        {
            ThrowIfNull(connectionMessageHandle);
            return Properties.GetProperty("connection.message.text.message");
        }

        /// <summary>
        /// Gets the binary message payload.
        /// </summary>
        /// <returns>An array of bytes containing the binary message.</returns>
        public byte[] GetBinaryMessage()
        {
            ThrowIfNull(connectionMessageHandle);

            var size = Internal.Connection.connection_message_get_data_size(connectionMessageHandle);

            IntPtr nativeBuffer = Marshal.AllocHGlobal((int)size);
            try
            {
                ThrowIfFail(Internal.Connection.connection_message_get_data(connectionMessageHandle, nativeBuffer, size));
                var buffer = new byte[size];
                Marshal.Copy(nativeBuffer, buffer, 0, (int)size);
                return buffer;
            }
            finally
            {
                Marshal.FreeHGlobal(nativeBuffer);
            }
        }

        /// <summary>
        /// A collection of properties and their values defined for this <see cref="ConnectionMessage"/>.
        /// Message headers can be accessed via this collection (e.g. "Content-Type").
        /// </summary>
        public PropertyCollection Properties { get; private set; }

        /// <summary>
        /// Returns a string that represents the connection message.
        /// </summary>
        /// <returns>A string that represents the connection message.</returns>
        public override string ToString()
        {
            return IsTextMessage()
                ? string.Format(CultureInfo.InvariantCulture, "Path:{0} Type:text Message:{1}", Path, GetTextMessage())
                : string.Format(CultureInfo.InvariantCulture, "Path:{0} Type:binary Size:{1}", Path, GetBinaryMessage().Length);
        }

        internal InteropSafeHandle connectionMessageHandle;
    }
}
