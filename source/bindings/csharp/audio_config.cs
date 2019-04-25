//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
    /// </summary>
    public sealed class AudioConfig : IDisposable
    {
        /// <summary>
        /// Creates an AudioConfig object representing the default microphone on the system.
        /// </summary>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromDefaultMicrophoneInput()
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_default_microphone(out audioConfigHandle));
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified file.
        /// </summary>
        /// <param name="fileName">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromWavFileInput(string fileName)
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_wav_file_name(out audioConfigHandle, fileName));
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="audioStream">Specifies the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(AudioInputStream audioStream)
        {
            ThrowIfNull(audioStream);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_stream(out audioConfigHandle, audioStream.StreamHandle));
            return new AudioConfig(audioConfigHandle, audioStream);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="callback">Specifies the pull audio input stream callback. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(PullAudioInputStreamCallback callback)
        {
            PullAudioInputStream pullStream = new PullAudioInputStream(callback);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_stream(out audioConfigHandle, pullStream.StreamHandle));
            return new AudioConfig(audioConfigHandle, pullStream, true);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="callback">Specifies the pull audio input stream callback. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamInput(PullAudioInputStreamCallback callback, AudioStreamFormat format)
        {
            PullAudioInputStream pullStream = new PullAudioInputStream(callback, format);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_stream(out audioConfigHandle, pullStream.StreamHandle));
            return new AudioConfig(audioConfigHandle, pullStream, true);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the designated input device.
        /// NOTE: This method was added in version 1.3.0.
        /// </summary>
        /// <param name="deviceName">Specifies the device name. Please refer to <a href="https://aka.ms/csspeech/microphone-selection">this page</a> on how to retrieve platform-specific microphone names.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromMicrophoneInput(string deviceName)
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_a_microphone(out audioConfigHandle, deviceName));
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the default speaker on the system.
        /// Added in version 1.4.0
        /// </summary>
        /// <returns>The audio output configuration being created.</returns>
        public static AudioConfig FromDefaultSpeakerOutput()
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_default_speaker(out audioConfigHandle));
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified file.
        /// Added in version 1.4.0
        /// </summary>
        /// <param name="fileName">Specifies the audio output file.</param>
        /// <returns>The audio output configuration being created.</returns>
        public static AudioConfig FromWavFileOutput(string fileName)
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_wav_file_name(out audioConfigHandle, fileName));
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// Added in version 1.4.0
        /// </summary>
        /// <param name="audioStream">Specifies the custom audio output stream.</param>
        /// <returns>The audio output configuration being created.</returns>
        public static AudioConfig FromStreamOutput(AudioOutputStream audioStream)
        {
            ThrowIfNull(audioStream);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_stream(out audioConfigHandle, audioStream.streamHandle));
            GC.KeepAlive(audioStream);
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// Added in version 1.4.0
        /// </summary>
        /// <param name="callback">Specifies the push audio output stream callback.</param>
        /// <returns>The audio output configuration being created.</returns>
        public static AudioConfig FromStreamOutput(PushAudioOutputStreamCallback callback)
        {
            PushAudioOutputStream pushStream = new PushAudioOutputStream(callback);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_stream(out audioConfigHandle, pushStream.streamHandle));
            return new AudioConfig(audioConfigHandle, pushStream, true);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// Added in version 1.4.0
        /// </summary>
        /// <param name="callback">Specifies the push audio output stream callback.</param>
        /// <param name="format">The audio data format in which audio will be sent to the push audio stream's write() method.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromStreamOutput(PushAudioOutputStreamCallback callback, AudioStreamFormat format)
        {
            PushAudioOutputStream pushStream = new PushAudioOutputStream(callback, format);
            IntPtr audioConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_stream(out audioConfigHandle, pushStream.streamHandle));
            return new AudioConfig(audioConfigHandle, pushStream, true);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            disposed = true;

            if (disposeStream)
            {
                streamKeepAlive.Dispose();
            }
            else
            {
                streamKeepAlive = null;
            }
        }

        private bool disposed = false;
        private IDisposable streamKeepAlive = null;
        private bool disposeStream = false;
        internal InteropSafeHandle configHandle;

        internal AudioConfig(IntPtr configPtr, AudioInputStream audioStream = null, bool ownStream = false)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.AudioConfig.audio_config_release);
            streamKeepAlive = audioStream;
            disposeStream = ownStream;
        }

        internal AudioConfig(IntPtr configPtr, AudioOutputStream audioStream, bool ownStream)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.AudioConfig.audio_config_release);
            streamKeepAlive = audioStream;
            disposeStream = ownStream;
        }
    }
}
