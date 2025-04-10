//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.IO;
using System.Text;

namespace Azure.AI.Test.Common.Servers.USP
{
    /// <summary>
    /// A Universal Speech Protocol (USP) message
    /// </summary>
    public abstract class Message
    {
        /// <summary>
        /// The number of bytes at the start of a binary message used to store the header size
        /// </summary>
        private const int BINARY_HEADER_SIZE_IN_BYTES = 2;

        /// <summary>
        /// The web socket sub protocol used when communicating with a USP endpoint
        /// </summary>
        public const string USP_WEB_SOCKET_PROTOCOL = "usp";

        /// <summary>
        /// The text encoding used in USP messages
        /// </summary>
        public static readonly Encoding TEXT_ENCODING = new UTF8Encoding(false);

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="isBinary">True if binary, false otherwise</param>
        protected Message(bool isBinary)
        {
            IsBinary = isBinary;
        }

        /// <summary>
        /// Gets the header information
        /// </summary>
        public Header Header { get; } = new Header();

        /// <summary>
        /// Gets whether or not this is a binary message
        /// </summary>
        public bool IsBinary { get; }

        /// <summary>
        /// Serializes the USP message to a byte array
        /// </summary>
        /// <returns></returns>
        public byte[] Serialize()
        {
            using (var stream = new MemoryStream())
            {
                Serialize(stream);
                return stream.ToArray();
            }
        }

        /// <summary>
        /// Serializes the USP message to the specified stream
        /// </summary>
        /// <param name="stream">The stream to write to</param>
        public void Serialize(Stream stream)
        {
            if (stream == null)
            {
                throw new ArgumentNullException(nameof(stream));
            }
            else if (!stream.CanWrite)
            {
                throw new IOException("Cannot write to the specified stream");
            }

            if (IsBinary)
            {
                // binary messages start with the first two bytes indicating the header size. We don't
                // want to assume the stream is seekable, so let's serialize the header to memory, use
                // that to determine the size and then copy the header data over to the actual stream

                // TODO improve by adding support for calculating the header size without serializing?
                using (var tempStream = new MemoryStream())
                {
                    Header.Serialize(tempStream);
                    tempStream.Flush();

                    long headerSize = tempStream.Position;
                    if (headerSize > ushort.MaxValue)
                    {
                        throw new InvalidOperationException("The USP header is too large");
                    }

                    byte[] headerSizeBytes = new byte[BINARY_HEADER_SIZE_IN_BYTES];
                    for (int i = 0; i < BINARY_HEADER_SIZE_IN_BYTES; i++)
                    {
                        int shift = 8 * (BINARY_HEADER_SIZE_IN_BYTES - i - 1);
                        headerSizeBytes[i] = (byte)((headerSize >> shift) & 0xff);
                    }

                    stream.Write(headerSizeBytes, 0, headerSizeBytes.Length);

                    tempStream.Seek(0, SeekOrigin.Begin);
                    tempStream.CopyTo(stream);
                }
            }
            else
            {
                Header.Serialize(stream);
            }

            SerializeData(stream);
        }

        /// <summary>
        /// Serializes the USP message data to the specified stream
        /// </summary>
        /// <param name="stream">The stream to write to</param>
        protected abstract void SerializeData(Stream stream);
    }
}
