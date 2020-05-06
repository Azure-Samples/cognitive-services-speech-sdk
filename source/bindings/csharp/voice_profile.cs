//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// A VoiceProfile represents a speaker's uniqueness in his/her voice.
    /// Added in version 1.12.0
    /// </summary>
    public sealed class VoiceProfile : DisposableBase
    {
        /// <summary>
        /// Creates a VoiceProfile.
        /// </summary>
        /// <param name="id">An unique id.</param>
        /// <returns></returns>
        ///
        public VoiceProfile(string id)
        {
            using (var idHandle = new Utf8StringHandle(id))
            {
                IntPtr handlePtr = Internal.SpeakerRecognition.SPXHANDLE_INVALID;
                ThrowIfFail(Internal.SpeakerRecognition.create_voice_profile_from_id(out handlePtr, idHandle));
                InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.SpeakerRecognition.voice_profile_release_handle);
                this.voiceProfileHandle = handle;
            }
        }

        internal VoiceProfile(InteropSafeHandle handle)
        {
            ThrowIfNull(handle, "Invalid voice profile handle");
            this.voiceProfileHandle = handle;
        }

        /// <summary>
        /// a voice profile id.
        /// </summary>
        public string Id
        {
            get
            {
                // get the length of the native string first
                uint length = 0;
                ThrowIfFail(Internal.SpeakerRecognition.voice_profile_get_id(voiceProfileHandle, Utf8StringHandle.Null, ref length));

                if (length < 0)
                {
                    return null;
                }
                else if (length == 0)
                {
                    return string.Empty;
                }

                // allocate enough native memory and read the string
                using (var stringHandle = new Utf8StringHandle(length))
                {
                    length = stringHandle.Length;
                    ThrowIfFail(Internal.SpeakerRecognition.voice_profile_get_id(voiceProfileHandle, stringHandle, ref length));
                    return stringHandle.ToString();
                }
            }
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
                    throw new InvalidOperationException("Cannot dispose a voice profile while an async action is running. Await async actions to avoid unexpected disposals.");
                }
                if (voiceProfileHandle != null)
                {
                    voiceProfileHandle.Dispose();
                }
            }
        }

        internal InteropSafeHandle voiceProfileHandle;
        private int activeAsyncCounter = 0;
    }
}
