//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMFORMATHANDLE = System.IntPtr;
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal class AudioInputStream : SpxExceptionThrower, IDisposable
    {
        protected SPXAUDIOCONFIGHANDLE streamHandle = IntPtr.Zero;
        protected bool disposed = false;
        protected bool isDisposing = false;
        protected readonly object thisLock = new object();

        internal AudioInputStream(IntPtr streamPtr)
        {
            streamHandle = streamPtr;
        }

        ~AudioInputStream()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            lock (thisLock)
            {
                isDisposing = true;
            }
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
            if (streamHandle != IntPtr.Zero)
            {
                LogErrorIfFail(audio_stream_release(streamHandle));
                streamHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static AudioStreamFormat UseDefaultFormatIfNull(AudioStreamFormat format)
        {
            return format != null ? format : AudioStreamFormat.DefaultInputFormat;
        }

        public SPXAUDIOSTREAMFORMATHANDLE StreamHandle
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
        public static extern bool audio_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE haudioStream);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_stream_release(SPXAUDIOSTREAMHANDLE haudioStream);

    }
}
