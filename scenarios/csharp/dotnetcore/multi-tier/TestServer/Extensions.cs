//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Azure.AI.Test.Common.Servers;
using System.Net;
using System.Net.Http.Headers;
using System.Net.WebSockets;
using System.Runtime.InteropServices;
using System.Text;

namespace Azure.AI.Test.Common.Utils
{
    /// <summary>
    /// Dictionary related extensions
    /// </summary>
    public static class DictionaryExtensions
    {
        /// <summary>
        /// Gets the value for the specified key from the dictionary, or returns the default value
        /// </summary>
        /// <typeparam name="TKey">The type of the key</typeparam>
        /// <typeparam name="TValue">The type of the value</typeparam>
        /// <param name="dict">The dictionary to retrieve values from</param>
        /// <param name="key">The key to retrieve</param>
        /// <param name="defaultValue">(Optional) The default value to return</param>
        /// <returns>The value stored for that key, or the default value otherwise</returns>
        public static TValue GetOrDefault<TKey, TValue>(this IDictionary<TKey, TValue> dict, TKey key, TValue defaultValue = default)
        {
            if (dict != null && key != null && dict.TryGetValue(key, out var value))
            {
                return value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Gets the value for the specified key from the read only dictionary, or returns the default value
        /// </summary>
        /// <typeparam name="TKey">The type of the key</typeparam>
        /// <typeparam name="TValue">The type of the value</typeparam>
        /// <param name="dict">The dictionary to retrieve values from</param>
        /// <param name="key">The key to retrieve</param>
        /// <param name="defaultValue">(Optional) The default value to return</param>
        /// <returns>The value stored for that key, or the default value otherwise</returns>
        public static TValue GetOrDefault<TKey, TValue>(this IReadOnlyDictionary<TKey, TValue> dict, TKey key, TValue defaultValue = default)
        {
            if (dict != null && key != null && dict.TryGetValue(key, out var value))
            {
                return value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Gets the value for the specified key from the dictionary, or returns the default value
        /// </summary>
        /// <typeparam name="TKey">The type of the key</typeparam>
        /// <typeparam name="TValue">The type of the value</typeparam>
        /// <param name="dict">The dictionary to retrieve values from</param>
        /// <param name="key">The key to retrieve</param>
        /// <param name="defaultValue">(Optional) The default value to return</param>
        /// <returns>The value stored for that key, or the default value otherwise</returns>
        public static TValue GetOrDefault<TKey, TValue>(this Dictionary<TKey, TValue> dict, TKey key, TValue defaultValue = default)
        {
            if (dict != null && key != null && dict.TryGetValue(key, out var value))
            {
                return value;
            }

            return defaultValue;
        }
    }

    /// <summary>
    /// Extensions for arrays
    /// </summary>
    public static class ArrayExtensions
    {
        /// <summary>
        /// Checks if the array starts with the specified sequence of bytes
        /// </summary>
        /// <param name="array">The array to check</param>
        /// <param name="count">The number of bytes in the array to check</param>
        /// <param name="other">The expected bytes</param>
        /// <returns>True if the array starts with the bytes specified in the other array, false otherwise</returns>
        public static bool StartsWith(this byte[] array, int count, byte[] other)
        {
            if (array == null || other == null || count < other.Length || count > array.Length)
            {
                return false;
            }

            for (int i = 0; i < other.Length; i++)
            {
                if (array[i] != other[i])
                {
                    return false;
                }
            }

            return true;
        }
    }

    /// <summary>
    /// Web socket related extensions
    /// </summary>
    public static class WebSocketExtensions
    {
        /// <summary>
        /// Sends the specified text as a text message using UTF-8 encoding
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="text">The text to send</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static Task SendAsync(this WebSocket ws, string text, CancellationToken token)
            => SendAsync(ws, text, Encoding.UTF8, token);

        /// <summary>
        /// Sends the specified text as a text message
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="text">The text to send</param>
        /// <param name="encoding">The text encoding to use to use</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static Task SendAsync(this WebSocket ws, string text, Encoding encoding, CancellationToken token)
        {
            encoding = encoding ?? Encoding.UTF8;
            var bytes = encoding.GetBytes(text);
            return ws.SendAsync(new ArraySegment<byte>(bytes), WebSocketMessageType.Text, true, token);
        }

        /// <summary>
        /// Sends the specified text as chunked text message using UTF-8 encoding
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="text">The text to send</param>
        /// <param name="chunkSizeInBytes">The maximum size in bytes of each message chunk</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static Task SendChunkedAsync(this WebSocket ws, string text, int chunkSizeInBytes, CancellationToken token)
            => SendChunkedAsync(ws, text, Encoding.UTF8, chunkSizeInBytes, token);

        /// <summary>
        /// Sends the specified text as chunked text message
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="text">The text to send</param>
        /// <param name="encoding">The text encoding to use to use</param>
        /// <param name="chunkSizeInBytes">The maximum size in bytes of each message chunk</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static Task SendChunkedAsync(this WebSocket ws, string text, Encoding encoding, int chunkSizeInBytes, CancellationToken token)
            => SendChunkedAsync(ws, encoding.GetBytes(text), chunkSizeInBytes, WebSocketMessageType.Text, token);

        /// <summary>
        /// Sends the specified bytes as chunked web socket messages
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="data">The data to send</param>
        /// <param name="chunkSizeInBytes">The maximum size in bytes of each message chunk</param>
        /// <param name="type">The type of web socket message</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static async Task SendChunkedAsync(this WebSocket ws, byte[] data, int chunkSizeInBytes, WebSocketMessageType type, CancellationToken token)
        {
            for (int offset = 0; offset < data.Length; offset += chunkSizeInBytes)
            {
                bool isEnd = offset + chunkSizeInBytes >= data.Length;
                int count = Math.Min(chunkSizeInBytes, data.Length - offset);

                await ws.SendAsync(
                    new ArraySegment<byte>(data, offset, count),
                    type,
                    isEnd,
                    token)
                .ConfigureAwait(false);
            }
        }

        /// <summary>
        /// Sends a USP message over the web socket
        /// </summary>
        /// <param name="ws">The web socket</param>
        /// <param name="uspMessage">The USP message to send</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        public static Task SendAsync(this WebSocket ws, Servers.USP.Message uspMessage, CancellationToken token)
        {
            return ws.SendAsync(
                new ArraySegment<byte>(uspMessage.Serialize()),
                uspMessage.IsBinary ? WebSocketMessageType.Binary : WebSocketMessageType.Text,
                true,
                token);
        }
    }
    
    /// <summary>
    /// Helper functions for ASP.Net MVC/WebAPI
    /// </summary>
    public static class AspExtensions
    {
#if NETFRAMEWORK
        /// <summary>
        /// Resolves a dependency
        /// </summary>
        /// <typeparam name="TType">The type of the dependency to retrieve</typeparam>
        /// <param name="resolver">The resolver to use</param>
        /// <returns>The resolved dependency or the default value</returns>
        public static TType GetService<TType>(this System.Web.Http.Dependencies.IDependencyResolver resolver)
        {
            return (TType)resolver?.GetService(typeof(TType));
        }
#endif
    }

    /// <summary>
    /// String extensions
    /// </summary>
    public static class StringExtensions
    {
        /// <summary>
        /// Ensures that the string starts with the specified prefix
        /// </summary>
        /// <param name="str">The string to examine</param>
        /// <param name="prefix">The prefix to ensure the string starts with</param>
        /// <returns>The string with the prefix at the start</returns>
        public static string EnsureStartsWith(this string str, string prefix)
        {
            if (str?.StartsWith(prefix) == true)
            {
                return str;
            }

            return prefix + str?.TrimStart();
        }
    }

    /// <summary>
    /// Serialization related extension methods
    /// </summary>
    public static class DeserializationExtensions
    {
        /// <summary>
        /// Parses an array of bytes into a struct. Please ensure that your struct has the correct
        /// padding
        /// </summary>
        /// <typeparam name="TStruct">The type of the struct</typeparam>
        /// <param name="bytes">The array of bytes to read</param>
        /// <returns>The parsed struct</returns>
        /// <exception cref="ArgumentNullException">If the byte array was null</exception>
        /// <exception cref="InvalidDataException">If there were not enough bytes to read the desired struct</exception>
        public static TStruct ReadAs<TStruct>(this byte[] bytes) where TStruct : struct
        {
            if (bytes == null)
            {
                throw new ArgumentNullException(nameof(bytes));
            }

            int structSizeInBytes = Marshal.SizeOf<TStruct>();
            if (bytes.Length < structSizeInBytes)
            {
                throw new InvalidDataException("There were not enough bytes to read the specified struct");
            }

            var handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            try
            {
                return Marshal.PtrToStructure<TStruct>(handle.AddrOfPinnedObject());
            }
            finally
            {
                handle.Free();
            }
        }

        /// <summary>
        /// Parses a segment of an array of bytes into a struct. Please ensure that your struct has
        /// the correct padding
        /// </summary>
        /// <typeparam name="TStruct">The type of the struct</typeparam>
        /// <param name="segment">The segment to read</param>
        /// <returns>The parsed struct</returns>
        public static TStruct ReadAs<TStruct>(this ArraySegment<byte> segment) where TStruct : struct
        {
            int structSizeInBytes = Marshal.SizeOf<TStruct>();
            if (segment.Count < structSizeInBytes)
            {
                throw new InvalidDataException("There were not enough bytes to read the specified struct");
            }

            var handle = GCHandle.Alloc(segment.Array, GCHandleType.Pinned);
            try
            {
                IntPtr offset = handle.AddrOfPinnedObject() + segment.Offset;
                return Marshal.PtrToStructure<TStruct>(offset);
            }
            finally
            {
                handle.Free();
            }
        }
    }

    /// <summary>
    /// Http client related extensions
    /// </summary>
    public static class HttpClientExtensions
    {
        private static readonly Uri DUMMY_URI = new Uri("https://not.a.real.uri.com");

        /// <summary>
        /// Sets the http client handler to use the system proxy
        /// </summary>
        /// <param name="handler">The HttpClientHandler instance</param>
        /// <returns>HttpClientHandler instance for chaining</returns>
        public static HttpClientHandler UseSystemProxy(this HttpClientHandler handler)
        {
            if (handler != null && TryGetDefaultSystemProxy(out var server, out var credentials))
            {
                var proxy = new WebProxy(server);
                if (credentials != null)
                {
                    proxy.Credentials = credentials;
                }
                else
                {
                    proxy.UseDefaultCredentials = true;
                }

                handler.UseProxy = true;
                handler.Proxy = proxy;
            }

            return handler;
        }

        /// <summary>
        /// Tries to get the system proxy to use
        /// </summary>
        /// <param name="server">The URI to the system proxy server</param>
        /// <param name="username">The username to use to authenticate to the proxy service. Will be null if none is needed</param>
        /// <param name="password">The password to use to authenticate to the proxy service. Will be null if none is needed</param>
        /// <returns>True if there was a system proxy set, false otherwise (even in the case of errors)</returns>
        public static bool TryGetDefaultSystemProxy(out Uri server, out string username, out string password)
        {
            username = null;
            password = null;

            try
            {
                bool ret = TryGetDefaultSystemProxy(out server, out var credentials);
                if (ret)
                {
                    var cred = credentials?.GetCredential(DUMMY_URI, string.Empty);
                    if (cred != null)
                    {
                        username = cred.UserName;
                        password = cred.Password;
                    }
                }

                return ret;
            }
            catch
            {
                server = null;
                username = null;
                password = null;
                return false;
            }
        }

        /// <summary>
        /// Gets a HTTP header value, or returns the default value if the header is not present
        /// </summary>
        /// <param name="headers">The http headers to look in</param>
        /// <param name="name">The name of the http header to return</param>
        /// <param name="default">The default value to return if the header is not present</param>
        /// <returns>The value of the first header with that name that is not null, empty, or whitespace only.
        /// If the header is not there, the default value is returned</returns>
        public static string GetOrDefault(this HttpHeaders headers, string name, string @default = null)
        {
            if (headers == null || name == null || !headers.TryGetValues(name, out var values))
            {
                return @default;
            }

            return values.FirstOrDefault(s => !string.IsNullOrWhiteSpace(s)) ?? @default;
        }

        private static bool TryGetDefaultSystemProxy(out Uri server, out ICredentials credentials)
        {
            server = null;
            credentials = null;

            try
            {
                IWebProxy webProxy = WebRequest.GetSystemWebProxy();

                Uri uri = webProxy?.GetProxy(DUMMY_URI);
                if (uri == null || uri == DUMMY_URI)
                {
                    return false;
                }

                server = uri;
                credentials = webProxy?.Credentials;
                return true;
            }
            catch
            {
                server = null;
                credentials = null;
                return false;
            }
        }
    }
}
