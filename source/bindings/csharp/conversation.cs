//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Perform conversation transcribing on the speech input. It returns recognized text and speaker id.
    /// Added in version 1.8.0
    /// </summary>
    public sealed class Conversation : DisposableBase
    {
        /// <summary>
        /// Creates a new conversation.
        /// </summary>
        /// <param name="speechConfig">The speech configuration to use.</param>
        /// <param name="conversationId">(Optional) The identifier for the conversation you want to join.</param>
        /// <returns></returns>
        public static Task<Conversation> CreateConversationAsync(SpeechConfig speechConfig, string conversationId = null)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            return Task.Run(() =>
            {
                IntPtr handlePtr = Internal.ConversationTranslator.SPXHANDLE_INVALID;
                using (var idHandle = new Utf8StringHandle(conversationId ?? string.Empty))
                {
                    ThrowIfFail(SpxFactory.conversation_create_from_config(out handlePtr, speechConfig.configHandle, idHandle));
                }

                InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.Conversation.conversation_release_handle);
                GC.KeepAlive(speechConfig);

                return new Conversation(handle);
            });
        }

        internal Conversation(InteropSafeHandle handle)
        {
            ThrowIfNull(handle, "Invalid conversation handle");

            this.conversationHandle = handle;

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Conversation.conversation_get_property_bag(handle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Gets/sets the conversation id.
        /// </summary>
        public string ConversationId
        {
            get
            {
                ThrowIfNull(conversationHandle);
                return SpxFactory.GetDataFromHandleUsingDelegate(Internal.Conversation.conversation_get_conversation_id, conversationHandle, maxCharCount);
            }
        }

        /// <summary>
        /// Gets/sets authorization token used to communicate with the service.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expires, the caller needs to refresh it by calling this setter with a new valid token.
        /// Otherwise, the recognizer will encounter errors during recognition.
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token);
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException(nameof(value));
                }

                Properties.SetProperty(PropertyId.SpeechServiceAuthorization_Token, value);
            }
        }

        /// <summary>
        /// Gets the language name that is used for recognition.
        /// </summary>
        public string SpeechRecognitionLanguage
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
            }
        }

        /// <summary>
        /// Gets the output format setting.
        /// </summary>
        public OutputFormat OutputFormat
        {
            get
            {
                return Properties.GetProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse, "false") == "true"
                    ? OutputFormat.Detailed
                    : OutputFormat.Simple;
            }
        }

        /// <summary>
        /// Gets the collection or properties and their values defined for this <see cref="ConversationTranscriber"/>.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Gets the native handle for the conversation.
        /// </summary>
        internal InteropSafeHandle NativeHandle => conversationHandle;

        /// <summary>
        /// Add a participant to a conversation using the user's id.
        ///
        /// Note: The returned participants can be used to remove. If the client changes the participant's attributes,
        /// the changed attributes are passed on to the service only when the participants is added again.
        ///
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task AddParticipantAsync(string userId)
        {
            return RunAsync((h) =>
            {
                var participant = Participant.From(userId, "", null);
                return Internal.Conversation.conversation_update_participant(h, true, participant.ParticipantHandle);
            });
        }

        /// <summary>
        /// Add a participant to a conversation using the User object.
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task AddParticipantAsync(User user) =>
            RunAsync((h) => Internal.Conversation.conversation_update_participant_by_user(h, true, user.userHandle));

        /// <summary>
        /// Add a participant to a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A participant object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task AddParticipantAsync(Participant participant)
            => RunAsync((h) => Internal.Conversation.conversation_update_participant(h, true, participant.ParticipantHandle));

        /// <summary>
        /// Remove a participant in a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A Participant object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task RemoveParticipantAsync(Participant participant)
            => RunAsync((h) => Internal.Conversation.conversation_update_participant(h, false, participant.ParticipantHandle));

        /// <summary>
        /// Remove a participant in a conversation using the User object
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>An asynchronous operation representing removing a participant.</returns>
        public Task RemoveParticipantAsync(User user)
            => RunAsync((h) => Internal.Conversation.conversation_update_participant_by_user(h, false, user.userHandle));

        /// <summary>
        /// Remove a participant from a conversation using a user id object
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>An asynchronous operation representing removing a participant.</returns>
        public Task RemoveParticipantAsync(string userId)
            => RunAsync((h) => Internal.Conversation.conversation_update_participant_by_user_id(h, false, userId));

        /// <summary>
        /// End a conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing ending a conversation.</returns>
        public Task EndConversationAsync() => RunAsync(Internal.Conversation.conversation_end_conversation);

        /// <summary>
        /// Start a conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing starting a conversation.</returns>
        public Task StartConversationAsync() => RunAsync(Internal.Conversation.conversation_start_conversation);

        /// <summary>
        /// Delete a conversation. After this no one will be able to join the conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing deleting a conversation.</returns>
        public Task DeleteConversationAsync() => RunAsync(Internal.Conversation.conversation_delete_conversation);

        /// <summary>
        /// Lock a conversation. This will prevent new participants from joining.
        /// </summary>
        /// <returns>An asynchronous operation representing locking a conversation.</returns>
        public Task LockConversationAsync() => RunAsync(Internal.Conversation.conversation_lock_conversation);

        /// <summary>
        /// Unlocks a conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing unlocking a conversation.</returns>
        public Task UnlockConversationAsync() => RunAsync(Internal.Conversation.conversation_unlock_conversation);

        /// <summary>
        /// Mute all other participants in the conversation. After this no other participants will
        /// have their speech recognitions broadcast, nor be able to send text messages.
        /// </summary>
        /// <returns>An asynchronous operation representing muting all participants.</returns>
        public Task MuteAllParticipantsAsync() => RunAsync(Internal.Conversation.conversation_mute_all_participants);

        /// <summary>
        /// Unmute all other participants in the conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing un-muting all participants.</returns>
        public Task UnmuteAllParticipantsAsync() => RunAsync(Internal.Conversation.conversation_unmute_all_participants);

        /// <summary>
        /// Mute a participant.
        /// </summary>
        /// <param name="userId">A user identifier.</param>
        /// <returns>An asynchronous operation representing muting a particular participant.</returns>
        public Task MuteParticipantAsync(string userId)
            => RunAsync((h) =>
            {
                using (var userIdHandle = new Utf8StringHandle(userId))
                {
                    return Internal.Conversation.conversation_mute_participant(h, userIdHandle);
                }
            });

        /// <summary>
        /// Unmute a participant.
        /// </summary>
        /// <param name="userId">A user identifier.</param>
        /// <returns>An asynchronous operation representing un-muting a particular participant.</returns>
        public Task UnmuteParticipantAsync(string userId)
            => RunAsync((h) =>
            {
                using (var userIdHandle = new Utf8StringHandle(userId))
                {
                    return Internal.Conversation.conversation_unmute_participant(h, userIdHandle);
                }
            });

        /// <summary>
        /// 
        /// </summary>
        /// <param name="disposeManaged"></param>
        override protected void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                // the C# standard guarantees that reads of 32 bit values always atomic
                if (activeAsyncCounter != 0)
                {
                    throw new InvalidOperationException("Cannot dispose a conversation while an async action is running. Await async actions to avoid unexpected disposals.");
                }

                // This will make Properties unaccessible.
                Properties.Close();
            }

            if (conversationHandle != null)
            {
                conversationHandle.Dispose();
            }
        }

        private const Int32 maxCharCount = 1024;

        private Task RunAsync(Func<InteropSafeHandle, IntPtr> method)
        {
            return Task.Run(() =>
            {
                CheckDisposed();
                ThrowIfNull(conversationHandle);

                try
                {
                    Interlocked.Increment(ref activeAsyncCounter);
                    ThrowIfFail(method(conversationHandle));
                }
                finally
                {
                    Interlocked.Decrement(ref activeAsyncCounter);
                }
            });
        }

        internal InteropSafeHandle conversationHandle;
        private int activeAsyncCounter = 0;
    }
}
