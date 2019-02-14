//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Threading;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;

    internal class AudioInputStream : IDisposable
    {
        protected InteropSafeHandle streamHandle;
        protected bool disposed = false;
        protected bool isDisposing = false;
        protected object thisLock = new object();

        internal AudioInputStream(IntPtr streamPtr)
        {
            streamHandle = new InteropSafeHandle(streamPtr, audio_stream_release);
        }

        public void Dispose()
        {
            isDisposing = true;
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            disposed = true;
        }

        public static AudioStreamFormat UseDefaultFormatIfNull(AudioStreamFormat format)
        {
            return format != null ? format : AudioStreamFormat.DefaultInputFormat;
        }

        public InteropSafeHandle StreamHandle
        {
            get
            {
                return streamHandle;
            }
        }

        public static PushAudioInputStream CreatePushStream()
        {
            return PushAudioInputStream.Create();
        }

        public static PushAudioInputStream CreatePushStream(AudioStreamFormat format)
        {
            return PushAudioInputStream.Create(format);
        }

        public static PullAudioInputStream CreatePullStream(PullAudioInputStreamCallback callback)
        {
            return PullAudioInputStream.Create(callback);
        }

        public static PullAudioInputStream CreatePullStream(AudioStreamFormat format, PullAudioInputStreamCallback callback)
        {
            return PullAudioInputStream.Create(format, callback);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_stream_is_handle_valid(InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_release(SPXAUDIOSTREAMHANDLE audioStream);

    }
}
