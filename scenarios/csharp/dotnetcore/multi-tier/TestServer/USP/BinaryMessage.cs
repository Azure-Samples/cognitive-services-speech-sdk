//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;

namespace Azure.AI.Test.Common.Servers.USP
{
    /// <summary>
    /// A USP binary message
    /// </summary>
    public class BinaryMessage : Message
    {
        /// <summary>
        /// Creates a new instance
        /// </summary>
        public BinaryMessage() : base(true)
        { }

        /// <summary>
        /// Creates a new instance by deserializing the binary data
        /// </summary>
        /// <param name="segment">The binary data to parse</param>
        public BinaryMessage(ArraySegment<byte> segment) : base(true)
        {
            if (segment.Array == null || segment.Count < 2)
            {
                return;
            }

            // binary messages have the first two bytes indicating the header size
            var headerSize = segment.Array[segment.Offset] * 256 + segment.Array[segment.Offset + 1];
            int headerOffset = segment.Offset + 2;

            if (headerSize > 0)
            {
                // make sure we don't get outside count
                headerSize = Math.Min(segment.Count - 2, headerSize);

                var headerText = TEXT_ENCODING.GetString(segment.Array, headerOffset, headerSize);
                Header.Deserialize(headerText);
            }

            int dataSize = segment.Count - 2 - headerSize;
            int dataOffset = headerOffset + headerSize;

            if (dataSize > 0)
            {
                // make sure we don't go outside count
                Data = new byte[dataSize];
                Array.Copy(segment.Array, dataOffset, Data, 0, dataSize);
            }
            else
            {
                Data = Array.Empty<byte>();
            }
        }

        /// <summary>
        /// Gets or sets the binary data
        /// </summary>
        public byte[] Data { get; set; }

        /// <inheritdoc />
        protected override void SerializeData(Stream stream)
        {
            if (Data != null)
            {
                stream.Write(Data, 0, Data.Length);
            }
        }
    }
}
