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

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// Perform conversation transcribing on the speech input. It returns recognized text and speaker id.
    /// Added in version 1.5.0
    /// </summary>
    public sealed class ConversationTranscriber : Recognizer
    {
        private event EventHandler<ConversationTranscriptionEventArgs> _Recognizing;
        private event EventHandler<ConversationTranscriptionEventArgs> _Recognized;
        private event EventHandler<ConversationTranscriptionCanceledEventArgs> _Canceled;

        /// <summary>
        /// The event <see cref="Transcribing"/> signals that an intermediate transcription result is received.
        /// </summary>
        public event EventHandler<ConversationTranscriptionEventArgs> Transcribing
        {
            add
            {
                if (this._Recognizing == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, recognizingCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognizing += value;
            }
            remove
            {
                this._Recognizing -= value;
                if (this._Recognizing == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_recognizing_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="Transcribed"/> signals that a final transcription result is received.
        /// </summary>
        public event EventHandler<ConversationTranscriptionEventArgs> Transcribed
        {
            add
            {
                if (this._Recognized == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, recognizedCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Recognized += value;
            }
            remove
            {
                this._Recognized -= value;
                if (this._Recognized == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_recognized_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The event <see cref="Canceled"/> signals that the speech recognition was canceled.
        /// </summary>
        public event EventHandler<ConversationTranscriptionCanceledEventArgs> Canceled
        {
            add
            {
                if (this._Canceled == null)
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, canceledCallbackDelegate, GCHandle.ToIntPtr(gch)));
                }
                this._Canceled += value;
            }
            remove
            {
                this._Canceled -= value;
                if (this._Canceled == null)
                {
                    LogErrorIfFail(Internal.Recognizer.recognizer_canceled_set_callback(recoHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// Creates a new instance of Conversation Transcriber.
        /// </summary>
        public ConversationTranscriber()
            :this(FromConfig(SpxFactory.recognizer_create_conversation_transcriber_from_config))
        {
        }

        /// <summary>
        /// Creates a new instance of ConversationTranscriber.
        /// </summary>
        /// <param name="audioConfig">Audio configuration</param>
        public ConversationTranscriber(Audio.AudioConfig audioConfig)
            : this(FromConfig(SpxFactory.recognizer_create_conversation_transcriber_from_config, audioConfig))
        {
        }

        internal delegate IntPtr GetTranscriberFromConfigDelegate(out IntPtr phreco, InteropSafeHandle audioInput);

        internal static InteropSafeHandle FromConfig(GetTranscriberFromConfigDelegate fromConfig, Audio.AudioConfig audioConfig)
        {
            if (audioConfig == null) throw new ArgumentNullException(nameof(audioConfig));

            IntPtr recoHandlePtr = IntPtr.Zero;
            ThrowIfFail(fromConfig(out recoHandlePtr, audioConfig.configHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.Recognizer.recognizer_handle_release);
            GC.KeepAlive(audioConfig);
            return recoHandle;
        }

        internal static InteropSafeHandle FromConfig(GetTranscriberFromConfigDelegate fromConfig )
        {

            IntPtr recoHandlePtr = IntPtr.Zero;
            IntPtr audioConfigPtr = IntPtr.Zero;
            InteropSafeHandle audioConfigHandle = new InteropSafeHandle(audioConfigPtr, null);
            ThrowIfFail(fromConfig(out recoHandlePtr, audioConfigHandle));
            InteropSafeHandle recoHandle = new InteropSafeHandle(recoHandlePtr, Internal.Recognizer.recognizer_handle_release);
            return recoHandle;
        }

        /// <summary>
        /// Join a conversation.
        /// </summary>
        /// <param name="conversation">The conversation to be joined.</param>
        /// <returns>An asynchronous operation representing joining a conversation.</returns>
        public Task JoinConversationAsync(Conversation conversation)
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(() =>
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_join_conversation(conversation.conversationHandle, recoHandle));
                });
            });
        }

        /// <summary>
        /// Leave a conversation.
        /// </summary>
        /// <returns>An asynchronous operation representing leaving a conversation.</returns>
        public Task LeaveConversationAsync()
        {
            return Task.Run(() =>
            {
                base.DoAsyncRecognitionAction(() =>
                {
                    ThrowIfFail(Internal.Recognizer.recognizer_leave_conversation(recoHandle));
                });
            });
        }

        internal ConversationTranscriber(InteropSafeHandle  recoHandle) : base(recoHandle)
        {
            recognizingCallbackDelegate = FireEvent_Recognizing;
            recognizedCallbackDelegate = FireEvent_Recognized;
            canceledCallbackDelegate = FireEvent_Canceled;

            ThrowIfNull(recoHandle, "Invalid ConversationTranscriber handle");

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.Recognizer.recognizer_get_property_bag(recoHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
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
        /// 
        /// </summary>
        /// <param name="disposing"></param>
        /// <returns></returns>
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

        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
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
                recognizer._Recognizing?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }

        // Defines a private methods to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.

        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
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
                recognizer._Recognized?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }

        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
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
                recognizer._Canceled?.Invoke(recognizer, resultEventArg);
            }
            catch (InvalidOperationException)
            {
                LogError(Internal.SpxError.InvalidHandle);
            }
        }
    }
}
