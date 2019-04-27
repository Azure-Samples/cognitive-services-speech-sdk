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

namespace Microsoft.CognitiveServices.Speech.Conversation
{
    /// <summary>
    /// Perform conversation transcribing on the speech input. It returns recognized text and speaker id.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class ConversationTranscriber : Recognizer
    {
        /// <summary>
        /// The event <see cref="Recognizing"/> signals that an intermediate recognition result is received.
        /// </summary>
        public event EventHandler<ConversationTranscriptionEventArgs> Recognizing;

        /// <summary>
        /// The event <see cref="Recognized"/> signals that a final recognition result is received.
        /// </summary>
        public event EventHandler<ConversationTranscriptionEventArgs> Recognized;

        /// <summary>
        /// The event <see cref="Canceled"/> signals that the speech recognition was canceled.
        /// </summary>
        public event EventHandler<ConversationTranscriptionCanceledEventArgs> Canceled;

        /// <summary>
        /// Creates a new instance of ConversationTranscriber.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        public ConversationTranscriber(SpeechConfig speechConfig)
            :this(FromConfig(SpxFactory.recognizer_create_conversation_transcriber_from_config, speechConfig))
        {
        }

        /// <summary>
        /// Creates a new instance of ConversationTranscriber.
        /// </summary>
        /// <param name="speechConfig">Speech configuration</param>
        /// <param name="audioConfig">Audio configuration</param>
        public ConversationTranscriber(SpeechConfig speechConfig, Audio.AudioConfig audioConfig)
            : this(FromConfig(SpxFactory.recognizer_create_conversation_transcriber_from_config, speechConfig, audioConfig))
        {
        }

        internal ConversationTranscriber(InteropSafeHandle  recoHandle) : base(recoHandle)
        {
            recognizingCallbackDelegate = FireEvent_Recognizing;
            recognizedCallbackDelegate = FireEvent_Recognized;
            canceledCallbackDelegate = FireEvent_Canceled;

            ThrowIfNull(recoHandle, "Invalid ConversationTranscriber handle");
            ThrowIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, recognizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
            ThrowIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, canceledCallbackDelegate, GCHandle.ToIntPtr(gch)));

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Recognizer.recognizer_get_property_bag(recoHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Gets/sets the conversation id.
        /// </summary>
        public string ConversationId
        {
            get
            {
                ThrowIfNull(recoHandle);
                return SpxFactory.GetDataFromHandleUsingDelegate(Internal.ConversationTranscriber.conversation_transcriber_get_conversation_id, recoHandle, maxCharCount);
            }
            set
            {
                ThrowIfNull(recoHandle);
                IntPtr nativePtr = Utf8StringMarshaler.MarshalManagedToNative(value);
                try
                {
                    ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_set_conversation_id(recoHandle, nativePtr));
                }
                finally
                {
                    Marshal.FreeHGlobal(nativePtr);
                }
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
        /// Starts conversation trancsribing on a continuous audio stream, until StopTranscribingAsync() is called.
        /// User must subscribe to events to receive recognition results.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that starts the recognition.</returns>
        public Task StartTranscribingAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(StartContinuousRecognition);
            });
        }

        /// <summary>
        /// Stops conversation transcribing.
        /// </summary>
        /// <returns>A task representing the asynchronous operation that stops the recognition.</returns>
        /// <remarks>This is used to pause the conversation. The client can start the conversation again by calling StartTranscribingAsync.</remarks>
        public Task StopTranscribingAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(StopContinuousRecognition);
            });
        }

        /// <summary>
        /// End a conversation.
        /// </summary>
        ///
        /// <returns>A task representing the asynchronous operation that ends the recognition.</returns>
        /// <remarks>This is used to communicate to the service to shutdown the conversation in the service.</remarks>
        public Task EndConversationAsync()
        {
            return Task.Run(() =>
            {
                ThrowIfNull(recoHandle);
                ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_end_conversation(recoHandle));
            });
        }

        /// <summary>
        /// Add a participant to a conversation using the user's id.
        ///
        /// Note: The returned participants can be used to remove. If the client changes the participant's attributes,
        /// the changed attributes are passed on to the service only when the participants is added again.
        ///
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>A Participant object.</returns>
        public Participant AddParticipant(string userId)
        {
            var participant = Participant.From(userId, "", null);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant(recoHandle, true, participant.participantHandle));
            return participant;
        }

        /// <summary>
        /// Add a participant to a conversation using the User object.
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>void.</returns>
        public void AddParticipant(User user)
        {
            ThrowIfNull(recoHandle);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant_by_user(recoHandle, true, user.userHandle));
        }

        /// <summary>
        /// Add a participant to a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A Participant object.</param>
        /// <returns>void.</returns>
        public void AddParticipant(Participant participant)
        {
            ThrowIfNull(recoHandle);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant(recoHandle, true, participant.participantHandle));
        }

        /// <summary>
        /// Remove a participant in a conversation using the Participant object
        /// </summary>
        /// <param name="participant">A Participant object.</param>
        /// <returns>void.</returns>
        public void RemoveParticipant(Participant participant)
        {
            ThrowIfNull(recoHandle);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant(recoHandle, false, participant.participantHandle));
        }

        /// <summary>
        /// Remove a participant in a conversation using the User object
        /// </summary>
        /// <param name="user">A User object.</param>
        /// <returns>void.</returns>
        public void RemoveParticipant(User user)
        {
            ThrowIfNull(recoHandle);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant_by_user(recoHandle, false, user.userHandle));
        }

        /// <summary>
        /// Remove a participant from a conversation using a user id object
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>void.</returns>
        public void RemoveParticipant(string userId)
        {
            ThrowIfNull(recoHandle);
            ThrowIfFail(Internal.ConversationTranscriber.conversation_transcriber_update_participant_by_user_id(recoHandle, false, userId));
        }

        protected override void Dispose(bool disposing)
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

            if (recoHandle != null)
            {
                LogErrorIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_session_started_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_session_stopped_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_speech_start_detected_set_callback(recoHandle, null, IntPtr.Zero));
                LogErrorIfFail(Internal.Recognizer.recognizer_speech_end_detected_set_callback(recoHandle, null, IntPtr.Zero));
                recoHandle.Dispose();
            }

            recognizingCallbackDelegate = null;
            recognizedCallbackDelegate = null;
            canceledCallbackDelegate = null;

            base.Dispose(disposing);
        }

        private Internal.CallbackFunctionDelegate recognizingCallbackDelegate;
        private Internal.CallbackFunctionDelegate recognizedCallbackDelegate;
        private Internal.CallbackFunctionDelegate canceledCallbackDelegate;

        private const Int32 maxCharCount = 1024;

        // Defines a private methods to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Recognizing(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<ConversationTranscriber>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new ConversationTranscriptionEventArgs(hevent);
                recognizer.Recognizing?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Recognized(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<ConversationTranscriber>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new ConversationTranscriptionEventArgs(hevent);
                recognizer.Recognized?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [Internal.MonoPInvokeCallback]
        private static void FireEvent_Canceled(IntPtr hreco, IntPtr hevent, IntPtr pvContext)
        {
            try
            {
                var recognizer = InteropSafeHandle.GetObjectFromWeakHandle<ConversationTranscriber>(pvContext);
                if (recognizer == null || recognizer.isDisposing)
                {
                    return;
                }
                var resultEventArg = new ConversationTranscriptionCanceledEventArgs(hevent);
                recognizer.Canceled?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }
    }
}
