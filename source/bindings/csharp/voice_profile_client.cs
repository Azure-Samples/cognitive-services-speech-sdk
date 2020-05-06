//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// A class for VoiceProfileClient.
    /// Added in version 1.12.0
    /// </summary>
    public sealed class VoiceProfileClient : DisposableBase
    {
        /// <summary>
        /// Creates a VoiceProfileClient.
        /// </summary>
        /// <param name="speechConfig">The speech configuration to use.</param>
        /// <returns></returns>
        ///
        public VoiceProfileClient(SpeechConfig speechConfig)
        {
            if (speechConfig == null) throw new ArgumentException(nameof(speechConfig));

            IntPtr handlePtr = Internal.SpeakerRecognition.SPXHANDLE_INVALID;

            ThrowIfFail(SpxFactory.create_voice_profile_client_from_config(out handlePtr, speechConfig.configHandle));

            InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.SpeakerRecognition.voice_profile_client_release_handle);
            GC.KeepAlive(speechConfig);

            this.voiceProfileClientHandle = handle;

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeakerRecognition.speaker_recognizer_get_property_bag(handle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Gets the collection of properties and their values defined for this <see cref="VoiceProfileClient"/>.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Create a voice profile.
        /// </summary>
        /// <param name="voiceProfileType">A voice profile type.</param>
        /// <param name="locale">a locale, e.g "en-us"</param>
        /// <returns>An asynchronous operation representing the result of creating a voice profile.</returns>
        public Task<VoiceProfile> CreateProfileAsync(VoiceProfileType voiceProfileType, string locale)
        {
            return RunAsync<VoiceProfile>((h) =>
            {
                IntPtr handlePtr = IntPtr.Zero;
                using (var localeHandle = new Utf8StringHandle(locale))
                {
                    Internal.SpeakerRecognition.create_voice_profile(voiceProfileClientHandle, (int)voiceProfileType, localeHandle, out handlePtr);

                    InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.SpeakerRecognition.voice_profile_release_handle);
                    return new VoiceProfile(handle);
                }
            });
        }

        /// <summary>
        /// Enroll a voice profile.
        /// </summary>
        /// <param name="voiceProfile">A voice profile.</param>
        /// <param name="audioConfig">An audio config.</param>
        /// <returns>An asynchronous operation representing the result of enrollment of a voice profile.</returns>
        public Task<VoiceProfileEnrollmentResult> EnrollProfileAsync(VoiceProfile voiceProfile, Audio.AudioConfig audioConfig)
        {
            return RunAsync<VoiceProfileEnrollmentResult>((h) =>
            {
                IntPtr resultHandle = IntPtr.Zero;

                Internal.SpeakerRecognition.enroll_voice_profile(h, voiceProfile.voiceProfileHandle, audioConfig.configHandle, out resultHandle);
                return new VoiceProfileEnrollmentResult(resultHandle);
            });
        }

        /// <summary>
        /// Delete a voice profile.
        /// </summary>
        /// <param name="voiceProfile">A voice profile.</param>
        /// <returns> An asynchronous operation representing the result of deleting a voice profile.</returns>
        public Task<VoiceProfileResult> DeleteProfileAsync(VoiceProfile voiceProfile)
        {
            return RunAsync<VoiceProfileResult>((h) =>
            {
                IntPtr resultHandle = IntPtr.Zero;

                Internal.SpeakerRecognition.delete_voice_profile(h, voiceProfile.voiceProfileHandle, out resultHandle);
                return new VoiceProfileResult(resultHandle);
            });
        }

        /// <summary>
        /// Reset a voice profile.
        /// </summary>
        /// <param name="voiceProfile">A voice profile.</param>
        /// <returns> An asynchronous operation representing the result of reset a voice profile.</returns>
        public Task<VoiceProfileResult> ResetProfileAsync(VoiceProfile voiceProfile)
        {
            return RunAsync<VoiceProfileResult>((h) =>
            {
                IntPtr resultHandle = IntPtr.Zero;

                Internal.SpeakerRecognition.reset_voice_profile(h, voiceProfile.voiceProfileHandle, out resultHandle);
                return new VoiceProfileResult(resultHandle);
            });
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="disposeManaged"></param>
        /// <returns></returns>
        override protected void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                // the C# standard guarantees that reads of 32 bit values always atomic
                if (activeAsyncCounter != 0)
                {
                    throw new InvalidOperationException("Cannot dispose a voice profile client while an async action is running. Await async actions to avoid unexpected disposals.");
                }

                // This will make Properties unaccessible.
                Properties.Close();

                if (voiceProfileClientHandle != null)
                {
                    voiceProfileClientHandle.Dispose();
                }
            }
        }

        private Task<ResultType> RunAsync<ResultType>(Func<InteropSafeHandle, ResultType> method)
        {
            return Task.Run(() =>
            {
                CheckDisposed();
                ThrowIfNull(voiceProfileClientHandle);
                ResultType result;
                try
                {
                    Interlocked.Increment(ref activeAsyncCounter);
                    result = method(voiceProfileClientHandle);
                }
                finally
                {
                    Interlocked.Decrement(ref activeAsyncCounter);
                }
                return result;
            });
        }

        internal InteropSafeHandle voiceProfileClientHandle;
        private int activeAsyncCounter = 0;
    }
}
