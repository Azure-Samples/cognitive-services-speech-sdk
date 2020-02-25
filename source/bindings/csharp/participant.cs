//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Represents a participant in a conversation.
    /// Changed in version 1.9.0
    /// </summary>
    public sealed class Participant : DisposableBase
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
            try
            {
                using (var userIdPtr = new Utf8StringHandle(userId))
                {
                    ThrowIfFail(Internal.Participant.participant_create_handle(out participantPtr, userIdPtr, preferredLanguage, voiceSignature));
                }

                return new Participant(participantPtr);
            }
            catch
            {
                if (participantPtr != IntPtr.Zero)
                {
                    LogErrorIfFail(Internal.Participant.participant_release_handle(participantPtr));
                }

                throw;
            }
        }

        /// <summary>
        /// Internal constructor
        /// </summary>
        internal Participant(IntPtr participantPtr)
        {
            if (participantPtr == null)
            {
                throw new ArgumentNullException(nameof(participantPtr));
            }

            ParticipantHandle = new InteropSafeHandle(participantPtr, Internal.Participant.participant_release_handle);

            IntPtr propbagPtr = IntPtr.Zero;
            ThrowIfFail(Internal.Participant.participant_get_property_bag(ParticipantHandle, out propbagPtr));
            Properties = new PropertyCollection(propbagPtr);

            Id = TryGetString(Internal.ConversationTranslator.conversation_translator_participant_get_id);
            Avatar = TryGetString(Internal.ConversationTranslator.conversation_translator_participant_get_avatar);
            DisplayName = TryGetString(Internal.ConversationTranslator.conversation_translator_participant_get_displayname);
            IsMuted = TryGetValue(Internal.ConversationTranslator.conversation_translator_participant_get_is_muted);
            IsUsingTts = TryGetValue(Internal.ConversationTranslator.conversation_translator_participant_get_is_using_tts);
            IsHost = TryGetValue(Internal.ConversationTranslator.conversation_translator_participant_get_is_host);
        }

        /// <summary>
        /// The unique identifier for the participant.
        /// </summary>
        public string Id { get; }

        /// <summary>
        /// Gets the colour of the user's avatar as an HTML hex string (e.g. FF0000 for red).
        /// </summary>
        public string Avatar { get; }

        /// <summary>
        /// The participant's display name. Please note that each participant within the same conversation must
        /// have a different display name. Duplicate names within the same conversation are not allowed. You can
        /// use the <see cref="Id"/> property as another way to refer to each participant.
        /// </summary>
        public string DisplayName { get; }

        /// <summary>
        /// Gets whether or not this participant is muted.
        /// </summary>
        public bool IsMuted { get; }

        /// <summary>
        /// Gets whether or not the participant is using Text To Speech (TTS).
        /// </summary>
        public bool IsUsingTts { get; }

        /// <summary>
        /// Gets whether or not this participant is the host.
        /// </summary>
        public bool IsHost { get; }

        /// <summary>
        /// The participant's preferred spoken language.
        /// </summary>
        public string PreferredLanguage
        {
            set
            {
                CheckDisposed();
                ThrowIfNull(ParticipantHandle);
                ThrowIfFail(Internal.Participant.participant_set_preferred_langugage(ParticipantHandle, value));
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
                CheckDisposed();
                ThrowIfNull(ParticipantHandle);
                var size = (value != null) ? value.Length : 0;
                ThrowIfFail(Internal.Participant.participant_set_voice_signature(ParticipantHandle, value, size));
            }
        }

        /// <summary>
        /// Contains properties of the participant.
        /// </summary>
        public PropertyCollection Properties { get; set; }

        /// <summary>
        /// Gets the native handle.
        /// </summary>
        internal InteropSafeHandle ParticipantHandle { get; }

        /// <summary>
        /// Disposes of the object.
        /// </summary>
        /// <param name="disposeManaged">True to dispose managed resources.</param>
        /// <returns></returns>
        protected override void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                ParticipantHandle?.Dispose();
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "Don't care about exceptions here")]
        private string TryGetString(Internal.ConversationTranslator.NativeReadString<InteropSafeHandle> nativeMethod)
        {
            try
            {
                return Internal.ConversationTranslator.GetString(ParticipantHandle, nativeMethod);
            }
            catch
            {
                // don't care about errors for now since not all participants will have these properties
                return null;
            }
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1031", Justification = "Don't care about exceptions here")]
        private bool TryGetValue(Internal.ConversationTranslator.NativeReadValue<InteropSafeHandle, bool> nativeMethod)
        {
            try
            {
                return Internal.ConversationTranslator.GetValue<bool>(ParticipantHandle, nativeMethod);
            }
            catch
            {
                // don't care about errors for now since not all participants will have these properties
                return false;
            }
        }
    }
}
