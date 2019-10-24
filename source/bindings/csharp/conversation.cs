//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Perform conversation transcribing on the speech input. It returns recognized text and speaker id.
    /// Added in version 1.8.0
    /// </summary>
    public sealed class Conversation : IDisposable
    {
        /// <summary>
        /// Creates a new instance of Conversation.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="conversationId">ConversationId</param>
        public Conversation(SpeechConfig speechConfig, string conversationId)
            :this(FromConfig(speechConfig, conversationId))
        {
        }

        /// <summary>
        /// Creates a new instance of Conversation.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        public Conversation(SpeechConfig speechConfig)
            : this(FromConfig(speechConfig))
        {
        }

        internal Conversation(InteropSafeHandle handle)
        {
            ThrowIfNull(handle, "Invalid conversation handle");

            this.conversationHandle = handle;
            gch = GCHandle.Alloc(this, GCHandleType.Weak);

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Conversation.conversation_get_property_bag(handle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        internal static InteropSafeHandle FromConfig(SpeechConfig speechConfig)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            string conversationId = string.Empty;
            IntPtr handlePtr = IntPtr.Zero;
            IntPtr idPtr = Utf8StringMarshaler.MarshalManagedToNative(conversationId);
            try
            {
                ThrowIfFail(SpxFactory.conversation_create_from_config(out handlePtr, speechConfig.configHandle, idPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(idPtr);
            }
            InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.Conversation.conversation_release_handle);
            GC.KeepAlive(speechConfig);

            return handle;
        }

        internal static InteropSafeHandle FromConfig(SpeechConfig speechConfig, string conversationId)
        {
            if (speechConfig == null)
            {
                throw new ArgumentNullException(nameof(speechConfig));
            }

            IntPtr handlePtr = IntPtr.Zero;
            IntPtr idPtr = Utf8StringMarshaler.MarshalManagedToNative(conversationId);
            try
            {
                ThrowIfFail(SpxFactory.conversation_create_from_config(out handlePtr, speechConfig.configHandle, idPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(idPtr);
            }

            InteropSafeHandle handle = new InteropSafeHandle(handlePtr, Internal.Conversation.conversation_release_handle);
            GC.KeepAlive(speechConfig);

            return handle;
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
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    var participant = Participant.From(userId, "", null);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant(conversationHandle, true, participant.participantHandle));
                });
            });
        }

        /// <summary>
        /// Add a participant to a conversation using the User object.
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task AddParticipantAsync(User user)
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant_by_user(conversationHandle, true, user.userHandle));
                });
            });

        }

        /// <summary>
        /// Add a participant to a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A participant object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task AddParticipantAsync(Participant participant)
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant(conversationHandle, true, participant.participantHandle));
                });
            });
        }

        /// <summary>
        /// Remove a participant in a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A Participant object.</param>
        /// <returns>An asynchronous operation representing adding a participant.</returns>
        public Task RemoveParticipantAsync(Participant participant)
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant(conversationHandle, false, participant.participantHandle));
                });
            });
        }

        /// <summary>
        /// Remove a participant in a conversation using the User object
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>An asynchronous operation representing removing a participant.</returns>
        public Task RemoveParticipantAsync(User user)
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant_by_user(conversationHandle, false, user.userHandle));
                });
            });
        }

        /// <summary>
        /// Remove a participant from a conversation using a user id object
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>An asynchronous operation representing removing a participant.</returns>
        public Task RemoveParticipantAsync(string userId)
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_update_participant_by_user_id(conversationHandle, false, userId));
                });
            });
        }

        /// <summary>
        /// End a conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing ending a participant.</returns>
        public Task EndConversationAsync()
        {
            return Task.Run(() =>
            {
                DoAsyncAction(() =>
                {
                    ThrowIfNull(conversationHandle);
                    ThrowIfFail(Internal.Conversation.conversation_end_conversation(conversationHandle));
                });
            });

        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            try
            {
                isDisposing = true;
                lock (conversationLock)
                {
                    if (activeAsyncCounter != 0)
                    {
                        throw new InvalidOperationException("Cannot dispose a conversation while async action is running. Await async actions to avoid unexpected disposals.");
                    }
                }
            }
            finally
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }
        }

        private void Dispose(bool disposing)
        {
            if (disposed)
            {
                return;
            }

            if (disposing)
            {
                // This will make Properties unaccessible.
                Properties.Close();
            }

            if (conversationHandle != null)
            {
                conversationHandle.Dispose();
            }
            if (gch.IsAllocated)
            {
                gch.Free();
            }
            disposed = true;
        }

        private const Int32 maxCharCount = 1024;

        private void DoAsyncAction(Action action)
        {
            lock (conversationLock)
            {
                activeAsyncCounter++;
            }
            if (disposed || isDisposing)
            {
                throw new ObjectDisposedException(this.GetType().Name);
            }
            try
            {
                action();
            }
            finally
            {
                lock (conversationLock)
                {
                    activeAsyncCounter--;
                }
            }
        }

        private GCHandle gch;
        volatile bool disposed = false;
        volatile bool isDisposing = false;
        internal InteropSafeHandle conversationHandle;
        readonly object conversationLock = new object();
        private int activeAsyncCounter = 0;
    }
}
