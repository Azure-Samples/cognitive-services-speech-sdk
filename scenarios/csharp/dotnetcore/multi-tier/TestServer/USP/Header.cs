//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Azure.AI.Test.Common.Utils;

namespace Azure.AI.Test.Common.Servers.USP
{
    /// <summary>
    /// Represents the headers of a USP message
    /// </summary>
    public class Header
    {
        /// <summary>
        /// USP header related constants
        /// </summary>
        public static class Constants
        {
            internal const string CONTENT_TYPE = "Content-Type";
            internal const string TIMESTAMP = "X-Timestamp";
            internal const string PATH = "Path";
            internal const string REQUEST_ID = "X-RequestId";
            internal const string STREAM_ID = "X-StreamId";
            internal const string PTS = "PTS";

            public const string JSON_CONTENT_TYPE = "application/json; charset=utf-8";
        }

        private const string FORMAT_TIMESTAMP = "yyyy-MM-ddTHH:mm:ss.fffZ";

        private static readonly string[] HEADER_SEPARATOR = new string[] { "\r\n", "\r", "\n" };
        private static readonly char[] HEADER_VALUE_SEPARATOR = new char[] { ':' };

        private Dictionary<string, string> _headers = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        private DateTimeOffset _ts;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        public Header()
        {
            Timestamp = DateTimeOffset.UtcNow;
        }

        /// <summary>
        /// Gets or sets the message path
        /// </summary>
        public string Path
        {
            get => _headers.GetOrDefault(Constants.PATH);
            set => SetOrDeleteHeader(Constants.PATH, value);
        }

        /// <summary>
        /// Gets or sets the unique identifier for the request
        /// </summary>
        public string RequestId
        {
            get => _headers.GetOrDefault(Constants.REQUEST_ID);
            set => SetOrDeleteHeader(Constants.REQUEST_ID, value);
        }

        /// <summary>
        /// Gets or sets the unique identifier for the stream. This will only be set on binary
        /// audio, or video messages. Returns -1 if the stream ID was not present
        /// </summary>
        public int StreamId
        {
            get
            {
                var strValue = _headers.GetOrDefault(Constants.STREAM_ID);
                if (int.TryParse(strValue, NumberStyles.Integer, CultureInfo.InvariantCulture, out var parsed))
                {
                    return parsed;
                }

                return -1;
            }

            set
            {
                SetOrDeleteHeader(Constants.STREAM_ID, value.ToString(CultureInfo.InvariantCulture));
            }
        }

        /// <summary>
        /// Gets or sets the presentation timestamp (PTS). This is a monotonically increasing value.
        /// Returns 0 if no presentation timestamp was present
        /// </summary>
        public ulong PresentationTimeStamp
        {
            get
            {
                var strValue = _headers.GetOrDefault(Constants.PTS);
                if (ulong.TryParse(strValue, NumberStyles.AllowLeadingWhite | NumberStyles.AllowTrailingWhite, CultureInfo.InvariantCulture, out var parsed))
                {
                    return parsed;
                }

                return 0;
            }

            set
            {
                SetOrDeleteHeader(Constants.PTS, value.ToString(CultureInfo.InvariantCulture));
            }
        }

        /// <summary>
        /// Gets or sets the time stamp
        /// </summary>
        public DateTimeOffset Timestamp
        {
            get => _ts;
            set
            {
                _ts = value;
                SetOrDeleteHeader(
                    Constants.TIMESTAMP,
                    value.ToUniversalTime().ToString(FORMAT_TIMESTAMP, CultureInfo.InvariantCulture));
            }
        }

        /// <summary>
        /// Gets or sets the content type
        /// </summary>
        public string ContentType
        {
            get => _headers.GetOrDefault(Constants.CONTENT_TYPE);
            set => SetOrDeleteHeader(Constants.CONTENT_TYPE, value);
        }

        /// <summary>
        /// Gets or sets an arbitrary header
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <returns>The value for that header</returns>
        public string this[string name]
        {
            get => _headers.GetOrDefault(name);
            set => SetOrDeleteHeader(name, value);
        }

        /// <summary>
        /// Checks whether or not the specified path is equal to the path of this message. This ignores
        /// case when checking
        /// </summary>
        /// <param name="path">The path to check</param>
        /// <returns>True if the paths are equal, false otherwise</returns>
        public bool IsPath(string path)
            => string.Equals(Path, path, StringComparison.OrdinalIgnoreCase);

        /// <summary>
        /// Deserializes the header portion
        /// </summary>
        /// <param name="headers">The headers to deserialize</param>
        internal void Deserialize(string headers)
        {
            if (headers == null)
            {
                return;
            }

            foreach (var header in headers.Split(HEADER_SEPARATOR, StringSplitOptions.RemoveEmptyEntries))
            {
                var parts = header?.Split(HEADER_VALUE_SEPARATOR, 2);
                if (parts?.Length == 2)
                {
                    _headers[parts[0]?.TrimEnd()] = parts[1]?.TrimStart();
                }
            }
        }

        /// <summary>
        /// Serializes the header to the specified stream
        /// </summary>
        /// <param name="stream">The stream to write to</param>
        internal void Serialize(Stream stream)
        {
            using (var writer = new StreamWriter(stream, Message.TEXT_ENCODING, 1024, true))
            {
                foreach (var header in _headers)
                {
                    writer.Write(header.Key ?? string.Empty);
                    writer.Write(HEADER_VALUE_SEPARATOR[0]);
                    writer.Write(header.Value ?? string.Empty);
                    writer.Write(HEADER_SEPARATOR[0]);
                }
            }
        }

        /// <summary>
        /// Sets a header value. If the value is null, the header is deleted
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The value to set. If null, the header will be removed</param>
        protected void SetOrDeleteHeader(string name, string value)
        {
            if (value == null)
            {
                _headers.Remove(name);
            }
            else
            {
                _headers[name] = value;
            }
        }
    }
}
