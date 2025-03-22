// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System.Text;

namespace SpeechSDKSamples.Remote.WebSocket.Shared
{
    public class MessageUtility
    {
        private static readonly string messageHeader = "SpeechSDKSamples.Remote.Messages";

        public static async Task<ReadOnlyMemory<byte>> SerializeMessageAsync(IMessage message, CancellationToken cancellationToken = default)
        {
            MemoryStream ms = new MemoryStream();
            await ms.WriteAsync(Encoding.UTF8.GetBytes(messageHeader), cancellationToken).ConfigureAwait(false);
            await ms.WriteAsync(Encoding.UTF8.GetBytes(Environment.NewLine), cancellationToken).ConfigureAwait(false);
            await ms.WriteAsync(Encoding.UTF8.GetBytes(message.GetType().AssemblyQualifiedName), cancellationToken).ConfigureAwait(false);
            await ms.WriteAsync(Encoding.UTF8.GetBytes(Environment.NewLine), cancellationToken).ConfigureAwait(false);
            await ms.WriteAsync(Encoding.UTF8.GetBytes(Environment.NewLine), cancellationToken).ConfigureAwait(false);
            await ms.WriteAsync(message.ToMemory(), cancellationToken);

            return ms.ToArray();
        }

        public static T DeserializeMessageByType<T>(ReadOnlyMemory<byte> bytes) where T : class
        {
            var headerOffset = FindHeaderEndOffset(bytes);
            if (headerOffset == -1)
            {
                return null;
            }

            var headerString = Encoding.UTF8.GetString(bytes.Slice(0, headerOffset).Span);
            var headers = headerString.Split(Environment.NewLine);

            if (headers.Length < 2)
            {
                return null;
            }

            if (headers[0] != messageHeader)
            {
                return null;
            }

            var messageClass = headers[1];
            var classType = Type.GetType(messageClass);

            if (classType == null || !typeof(T).IsAssignableFrom(classType))
            {
                return null;
            }

            var constructor = classType?.GetConstructor(new Type[] { typeof(ReadOnlyMemory<byte>) });

            var payload = bytes.Slice(headerOffset, bytes.Length - headerOffset);

            T ret = constructor?.Invoke(new object[] { payload }) as T;

            return ret;
        }

        public static IMessage DeserializeMessage(ReadOnlyMemory<byte> bytes)
        {
            return DeserializeMessageByType<IMessage>(bytes);
        }

        private static int FindHeaderEndOffset(ReadOnlyMemory<byte> bytes)
        {
            // Next we need to find the typename.
            var newline = Encoding.UTF8.GetBytes(Environment.NewLine + Environment.NewLine);

            int headerEndOffset = -1;

            if (bytes.Length <= newline.Length)
            {
                return headerEndOffset;
            }

            for (int i = 0; i <= bytes.Length - newline.Length; i++)
            {
                bool match = true;
                int current = i;

                for (int j = 0; j < newline.Length; j++)
                {
                    if (bytes.Span[current++] != newline[j])
                    {
                        match = false;
                        break;
                    }
                }

                if (match)
                {
                    headerEndOffset = i;
                    break;
                }
            }

            return headerEndOffset + newline.Length;
        }
    }
}
