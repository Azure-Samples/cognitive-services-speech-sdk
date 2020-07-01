//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Represents speaker verification model.
    /// Added in 1.12.0
    /// </summary>
    public sealed class SpeakerVerificationModel : IDisposable
    {
        /// <summary>
        /// Creates a speaker verification model from the voice profile.
        /// </summary>
        /// <param name="profile">A voice profile.</param>
        /// <returns>The speaker verification model being created.</returns>
        public static SpeakerVerificationModel FromProfile(VoiceProfile profile)
        {
            ThrowIfNull(profile);
            IntPtr modelHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SpeakerRecognition.speaker_verification_model_create(out modelHandle, profile.voiceProfileHandle));
            return new SpeakerVerificationModel(modelHandle);
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                modelHandle.Dispose();
            }
            // dispose unmanaged resources
            disposed = true;
            GC.SuppressFinalize(this);
        }

        private SpeakerVerificationModel(IntPtr modelHandlePtr)
        {
            modelHandle = new InteropSafeHandle(modelHandlePtr, Internal.SpeakerRecognition.speaker_verification_model_release_handle);
        }

        private bool disposed = false;
        internal InteropSafeHandle modelHandle;
    }
}
