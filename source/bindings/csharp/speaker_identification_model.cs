//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Represents speaker identification model.
    /// Added in version 1.12.0
    /// </summary>
    public sealed class SpeakerIdentificationModel : IDisposable
    {
        /// <summary>
        /// Creates a speaker identification model from the voice profiles.
        /// </summary>
        /// <param name="profiles">a collection of voice profiles.</param>
        /// <returns>The speaker identification model being created.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static SpeakerIdentificationModel FromProfiles(IEnumerable<VoiceProfile> profiles)
        {
            IntPtr modelHandle = IntPtr.Zero;
            ThrowIfNull(profiles, "profiles cannot be null");
            ThrowIfFail(Internal.SpeakerRecognition.speaker_identification_model_create(out modelHandle));

            InteropSafeHandle handle = new InteropSafeHandle(modelHandle, Internal.SpeakerRecognition.speaker_identification_model_release_handle);

            foreach (var profile in profiles)
            {
                ThrowIfFail(Internal.SpeakerRecognition.speaker_identification_model_add_profile(handle, profile.voiceProfileHandle));
            }
            return new SpeakerIdentificationModel(handle);
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

        private SpeakerIdentificationModel(InteropSafeHandle handle)
        {
            modelHandle = handle;
        }

        private bool disposed = false;
        internal InteropSafeHandle modelHandle;
    }
}
