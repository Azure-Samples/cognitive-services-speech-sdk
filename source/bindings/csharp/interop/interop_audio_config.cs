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
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal class AudioConfig: SpxExceptionThrower, IDisposable
    {
        internal SPXAUDIOCONFIGHANDLE configHandle = IntPtr.Zero;
        private bool disposed = false;

        internal AudioConfig(IntPtr configPtr)
        {
            configHandle = configPtr;
        }

        ~AudioConfig()
        {
            Dispose(false);
        }

        public void Dispose()
        {
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
            if (configHandle != IntPtr.Zero)
            {
                LogErrorIfFail(audio_config_release(configHandle));
                configHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static AudioConfig FromDefaultMicrophoneInput()
        {
            SPXAUDIOCONFIGHANDLE audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(audio_config_create_audio_input_from_default_microphone(out audioConfigHandle));
            return new AudioConfig(audioConfigHandle);
        }

        public static AudioConfig FromWavFileInput(string fileName)
        {
            SPXAUDIOCONFIGHANDLE audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(audio_config_create_audio_input_from_wav_file_name(out audioConfigHandle, fileName));
            return new AudioConfig(audioConfigHandle);
        }

        public static AudioConfig FromMicrophoneInput(string deviceName)
        {
            SPXAUDIOCONFIGHANDLE audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(audio_config_create_audio_input_from_a_microphone(out audioConfigHandle, deviceName));
            return new AudioConfig(audioConfigHandle);
        }

        public static AudioConfig FromStreamInput(AudioInputStream stream)
        {
            ThrowIfNull(stream);
            SPXAUDIOCONFIGHANDLE audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(audio_config_create_audio_input_from_stream(out audioConfigHandle, stream.StreamHandle));
            GC.KeepAlive(stream);
            return new AudioConfig(audioConfigHandle);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_config_is_handle_valid(SPXAUDIOCONFIGHANDLE haudioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_input_from_default_microphone(out SPXAUDIOCONFIGHANDLE haudioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_wav_file_name(out SPXAUDIOCONFIGHANDLE haudioConfig,
            [MarshalAs(UnmanagedType.LPStr)] string fileName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_a_microphone(out SPXAUDIOCONFIGHANDLE haudioConfig,
            [MarshalAs(UnmanagedType.LPStr)] string deviceName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_input_from_stream(out SPXAUDIOCONFIGHANDLE haudioConfig, SPXAUDIOSTREAMHANDLE haudioStream);        
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_release(SPXAUDIOCONFIGHANDLE haudioConfig);
    }
}
