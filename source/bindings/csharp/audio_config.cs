//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Audio
{
    /// <summary>
    /// Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2213", MessageId = "streamKeepAlive", Justification = "The audio stream is sometimes not owned, and should not be disposed.")]
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
        /// <param name="fileName">Specifies the audio input file.</param>
        /// <returns>The audio input configuration being created.</returns>
        public static AudioConfig FromWavFileInput(string fileName)
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            IntPtr fileNamePtr = Utf8StringMarshaler.MarshalManagedToNative(fileName);
            try
            {
                ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_input_from_wav_file_name(out audioConfigHandle, fileNamePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(fileNamePtr);
            }
            return new AudioConfig(audioConfigHandle);
        }

        /// <summary>
        /// Creates an AudioConfig object representing the specified stream.
        /// </summary>
        /// <param name="audioStream">Specifies the custom audio input stream.</param>
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
        /// <param name="callback">Specifies the pull audio input stream callback.</param>
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
        /// <param name="callback">Specifies the pull audio input stream callback.</param>
        /// <param name="format">The audio data format in which audio will be written to the push audio stream's write() method.</param>
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
        /// <param name="fileName">Specifies the audio output file. The parent directory must already exist.</param>
        /// <returns>The audio output configuration being created.</returns>
        public static AudioConfig FromWavFileOutput(string fileName)
        {
            IntPtr audioConfigHandle = IntPtr.Zero;
            IntPtr fileNamePtr = Utf8StringMarshaler.MarshalManagedToNative(fileName);
            try
            {
                ThrowIfFail(Internal.AudioConfig.audio_config_create_audio_output_from_wav_file_name(out audioConfigHandle, fileNamePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(fileNamePtr);
            }
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
            return new AudioConfig(audioConfigHandle, audioStream, false);
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
        /// Dispose of associated resources.
        /// We may or may not dispose the audio stream, depending if we are told we own the stream give to us.
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

            propBag?.Close();
        }

        /// <summary>
        /// Sets the property by name.
        /// Added in version 1.10.0.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(string name, string value)
        {
            if (value == null)
            {
                throw new ArgumentNullException("Invalid property value.");
            }

            propBag.SetProperty(name, value);
        }

        /// <summary>
        /// Sets the property by propertyId
        /// Added in version 1.10.0.
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <param name="value">Value of the property</param>
        public void SetProperty(PropertyId id, string value)
        {
            if (value == null)
            {
                throw new ArgumentNullException("Invalid property value.");
            }

            propBag.SetProperty(id, value);
        }

        /// <summary>
        /// Gets the property by name.
        /// Added in version 1.10.0.
        /// </summary>
        /// <param name="name">Name of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(string name)
        {
            return propBag.GetProperty(name);
        }

        /// <summary>
        /// Gets the property by propertyId
        /// Added in version 1.10.0.
        /// </summary>
        /// <param name="id">PropertyId of the property</param>
        /// <returns>Value of the property</returns>
        public string GetProperty(PropertyId id)
        {
            return propBag.GetProperty(id);
        }

        private bool disposed = false;
        private IDisposable streamKeepAlive = null;
        private bool disposeStream = false;
        internal InteropSafeHandle configHandle;
        private PropertyCollection propBag;

        internal AudioConfig(IntPtr configPtr, AudioInputStream audioStream = null, bool ownStream = false)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.AudioConfig.audio_config_release);
            streamKeepAlive = audioStream;
            disposeStream = ownStream;

            IntPtr hpropbag = IntPtr.Zero;
            ThrowIfFail(Internal.AudioConfig.audio_config_get_property_bag(configPtr, out hpropbag));
            propBag = new PropertyCollection(hpropbag);
        }

        internal AudioConfig(IntPtr configPtr, AudioOutputStream audioStream, bool ownStream)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.AudioConfig.audio_config_release);
            streamKeepAlive = audioStream;
            disposeStream = ownStream;
        }

        internal IDisposable MoveStreamOwnerShip()
        {
            if (disposeStream)
            {
                disposeStream = false;
                return streamKeepAlive;
            }
            return null;
        }
    }
}
