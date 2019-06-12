//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Conversation
{
    /// <summary>
    /// Represents a participant in a conversation.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class Participant
    {
        /// <summary>
        /// Creates a Participant using user id, her/his preferred language and her/his voice signature.
        /// If voice signature is empty then user will not be identified.
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <param name="preferredLanguage">A preferred language.</param>
        /// <param name="voiceSignature">A voice signature of the user.</param>
        /// <returns>A Participant object</returns>
        public static Participant From(string userId, string preferredLanguage, string voiceSignature)
        {
            IntPtr participantPtr = IntPtr.Zero;
            IntPtr userIdPtr = Utf8StringMarshaler.MarshalManagedToNative(userId);
            try
            {
                ThrowIfFail(Internal.Participant.participant_create_handle(out participantPtr, userIdPtr, preferredLanguage, voiceSignature));
            }
            finally
            {
                Marshal.FreeHGlobal(userIdPtr);
            }
            return new Participant(participantPtr);
        }

        /// <summary>
        /// Internal constructor
        /// </summary>
        internal Participant(IntPtr participantPtr)
        {
            ThrowIfNull(participantPtr);
            participantHandle = new InteropSafeHandle(participantPtr, Internal.Participant.participant_release_handle);

            IntPtr propbagPtr = IntPtr.Zero;
            ThrowIfFail(Internal.Participant.participant_get_property_bag(participantHandle, out propbagPtr));
            Properties = new PropertyCollection(propbagPtr);
        }

        /// <summary>
        /// The participant's preferred spoken language.
        /// </summary>
        public string PreferredLanguage
        {
            set
            {
                ThrowIfNull(participantHandle);
                ThrowIfFail(Internal.Participant.participant_set_preferred_langugage(participantHandle, value));
            }
        }

        /// <summary>
        /// The participant's voice signature.
        /// If voice signature is empty then user will not be identified.
        /// </summary>
        public string VoiceSignature
        {
            set
            {
                ThrowIfNull(participantHandle);
                var size = (value != null) ? value.Length : 0;
                ThrowIfFail(Internal.Participant.participant_set_voice_signature(participantHandle, value, size));
            }
        }

        /// <summary>
        /// Contains properties of the participant.
        /// </summary>
        public PropertyCollection Properties { get; set; }

        // Hold the reference.
        internal InteropSafeHandle participantHandle;
    }
}
