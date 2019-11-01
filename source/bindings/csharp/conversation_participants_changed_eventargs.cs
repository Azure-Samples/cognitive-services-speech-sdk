//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using static Microsoft.CognitiveServices.Speech.Internal.ConversationTranslator;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Why the participant changed event was raised
    /// Added in version 1.8.0
    /// </summary>
    public enum ParticipantChangedReason
    {
        /// <summary>
        /// Participant has joined the conversation.
        /// </summary>
        JoinedConversation,

        /// <summary>
        /// Participant has left the conversation. This could be voluntary, or involuntary
        /// (e.g. they are experiencing networking issues).
        /// </summary>
        LeftConversation,

        /// <summary>
        /// The participants' state has changed (e.g. they became muted, changed their nickname).
        /// </summary>
        Updated
    };

    /// <summary>
    /// Defines a payload for the <see cref="ConversationTranslator.ParticipantsChanged" /> event.
    /// </summary>
    public class ConversationParticipantsChangedEventArgs : SessionEventArgs
    {
        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="handle">The event handle.</param>
        internal ConversationParticipantsChangedEventArgs(IntPtr handle) : base(handle)
        {
            Reason = GetValue<ParticipantChangedReason>(handle, conversation_translator_event_get_participant_changed_reason);

            var participants = new List<Participant>();
            IntPtr participantHandle = IntPtr.Zero;

            try
            {
                for (int i = 0; participantHandle != SPXHANDLE_INVALID; i++)
                {
                    ThrowIfFail(conversation_translator_event_get_participant_changed_at_index(handle, i, out participantHandle));
                    if (participantHandle != SPXHANDLE_INVALID)
                    {
                        participants.Add(new Participant(participantHandle));
                        participantHandle = IntPtr.Zero;
                    }
                }
            }
            catch
            {
                // dispose of any created participants
                foreach (var p in participants)
                {
                    p.Dispose();
                }

                throw;
            }
            finally
            {
                if (participantHandle != SPXHANDLE_INVALID && participantHandle != IntPtr.Zero)
                {
                    ThrowIfFail(Internal.Participant.participant_release_handle(participantHandle));
                }
            }

            Participants = participants;
        }

        /// <summary>
        /// Why the participant changed event was raised (e.g. a participant joined).
        /// </summary>
        public ParticipantChangedReason Reason { get; }

        /// <summary>
        /// The participant(s) that joined, left, or were updated.
        /// </summary>
        public IReadOnlyList<Participant> Participants;
    }
}
