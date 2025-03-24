//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;

namespace Azure.AI.Test.Common.Servers.USP
{
    /// <summary>
    /// A USP text message
    /// </summary>
    public class TextMessage : Message
    {
        internal const string BODY_SEPARATOR = "\r\n\r\n";

        /// <summary>
        /// Creates a new instance
        /// </summary>
        public TextMessage() : base(false)
        {
        }

        /// <summary>
        /// Creates a new instance by deserializing the UTF-8 byte data
        /// </summary>
        /// <param name="data">The UTF-8 data to parse</param>
        public TextMessage(ArraySegment<byte> data) : base(false)
        {
            // TODO read as stream instead?
            var message = TEXT_ENCODING.GetString(data.Array, data.Offset, data.Count);
            ParseStringData(message);
        }

        /// <summary>
        /// Creates a new instance by parsing the raw text data
        /// </summary>
        /// <param name="message">The raw string data to parse</param>
        public TextMessage(string message) : base(false)
        {
            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            ParseStringData(message);
        }

        /// <summary>
        /// Gets the string data
        /// </summary>
        public string Data { get; set; }

        /// <inheritdoc />
        protected override void SerializeData(Stream stream)
        {
            using (var writer = new StreamWriter(stream, TEXT_ENCODING, 1024, true))
            {
                writer.Write("\r\n");
                writer.Write(Data ?? string.Empty);
            }
        }

        private void ParseStringData(string message)
        {
            var headerEnd = message.IndexOf(BODY_SEPARATOR);

            var headerText = headerEnd >= 0
                ? message.Substring(0, headerEnd)
                : string.Empty;
            Header.Deserialize(headerText);

            Data = headerEnd < 0
                ? message
                : message.Substring(headerEnd + BODY_SEPARATOR.Length);
        }
    }
}
