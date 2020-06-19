//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;
using System.Threading;
using System.Runtime.CompilerServices;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Transcription
{
    /// <summary>
    /// A conversation translator that enables a connected experience where participants can use their
    /// own devices to see everyone else's recognitions and IMs in their own languages. Participants
    /// can also speak and send IMs to others.
    /// Added in 1.9.0
    /// </summary>
    public sealed class ConversationTranslator : Internal.DisposableBase
    {
        internal Internal.InteropSafeHandle _nativeHandle;
        private int _activeAsyncCounter = 0;

        private InteropEvent<SessionEventArgs> _sessionStarted;
        private InteropEvent<SessionEventArgs> _sessionStopped;
        private InteropEvent<ConversationTranslationCanceledEventArgs> _canceled;
        private InteropEvent<ConversationParticipantsChangedEventArgs> _participantsChanged;
        private InteropEvent<ConversationExpirationEventArgs> _conversationExpiration;
        private InteropEvent<ConversationTranslationEventArgs> _transcribing;
        private InteropEvent<ConversationTranslationEventArgs> _transcribed;
        private InteropEvent<ConversationTranslationEventArgs> _textMessageReceived;

        private Audio.AudioConfig _audioInputKeepAlive;

        /// <summary>
        /// Creates a new instance of the Conversation Translator using the default microphone input.
        /// </summary>
        public ConversationTranslator()
            : this(Create(null, false))
        {
        }

        /// <summary>
        /// Creates a new instance of the Conversation Translator.
        /// </summary>
        /// <param name="audioConfig">Audio configuration.</param>
        /// <exception cref="ArgumentNullException">If the audioConfig is null.</exception>
        public ConversationTranslator(Audio.AudioConfig audioConfig)
            : this(Create(audioConfig, true))
        {
            _audioInputKeepAlive = audioConfig;
        }

        /// <summary>
        /// Creates a new instance.
        /// </summary>
        /// <param name="nativeHandle">The handle to the native conversation translator.</param>
        internal ConversationTranslator(Internal.InteropSafeHandle nativeHandle)
        {
            _nativeHandle = nativeHandle ?? throw new ArgumentNullException(nameof(nativeHandle));

            _sessionStarted = new InteropEvent<SessionEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_session_started_set_callback);
            _sessionStopped = new InteropEvent<SessionEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_session_stopped_set_callback);
            _canceled = new InteropEvent<ConversationTranslationCanceledEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_canceled_set_callback);
            _participantsChanged = new InteropEvent<ConversationParticipantsChangedEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_participants_changed_set_callback);
            _conversationExpiration = new InteropEvent<ConversationExpirationEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_conversation_expiration_set_callback);
            _transcribing = new InteropEvent<ConversationTranslationEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_transcribing_set_callback);
            _transcribed = new InteropEvent<ConversationTranslationEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_transcribed_set_callback);
            _textMessageReceived = new InteropEvent<ConversationTranslationEventArgs>(
                this, _nativeHandle, Internal.ConversationTranslator.conversation_translator_text_message_recevied_set_callback);

            IntPtr propertyHandle = IntPtr.Zero;
            ThrowIfFail(Internal.ConversationTranslator.conversation_translator_get_property_bag(nativeHandle, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        /// <summary>
        /// Event that signals the start of a conversation translation session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStarted
        {
            add { _sessionStarted?.Add(value); }
            remove { _sessionStarted.Remove(value); }
        }

        /// <summary>
        /// Event that signals the end of a conversation translation session.
        /// </summary>
        public event EventHandler<SessionEventArgs> SessionStopped
        {
            add { _sessionStopped?.Add(value); }
            remove { _sessionStopped.Remove(value); }
        }

        /// <summary>
        /// Event that signals an error with the conversation transcription, or the end of the
        /// audio stream has been reached.
        /// </summary>
        public event EventHandler<ConversationTranslationCanceledEventArgs> Canceled
        {
            add { _canceled?.Add(value); }
            remove { _canceled.Remove(value); }
        }

        /// <summary>
        /// Event that signals participants in the room have changed (e.g. a new participant joined).
        /// </summary>
        public event EventHandler<ConversationParticipantsChangedEventArgs> ParticipantsChanged
        {
            add { _participantsChanged?.Add(value); }
            remove { _participantsChanged.Remove(value); }
        }

        /// <summary>
        /// Event that signals how many more minutes are left before the conversation expires.
        /// </summary>
        public event EventHandler<ConversationExpirationEventArgs> ConversationExpiration
        {
            add { _conversationExpiration?.Add(value); }
            remove { _conversationExpiration.Remove(value); }
        }

        /// <summary>
        /// Event that signals an intermediate conversation translation result is available for a
        /// conversation participant.
        /// </summary>
        public event EventHandler<ConversationTranslationEventArgs> Transcribing
        {
            add { _transcribing?.Add(value); }
            remove { _transcribing.Remove(value); }
        }

        /// <summary>
        /// Event that signals a final conversation translation result is available for a conversation
        /// participant.
        /// </summary>
        public event EventHandler<ConversationTranslationEventArgs> Transcribed
        {
            add { _transcribed?.Add(value); }
            remove { _transcribed.Remove(value); }
        }

        /// <summary>
        /// Event that signals a translated text message from a conversation participant.
        /// </summary>
        public event EventHandler<ConversationTranslationEventArgs> TextMessageReceived
        {
            add { _textMessageReceived?.Add(value); }
            remove { _textMessageReceived.Remove(value); }
        }

        /// <summary>
        /// Gets the language name that is used for recognition.
        /// </summary>
        public string SpeechRecognitionLanguage
            => Properties.GetProperty(PropertyId.SpeechServiceConnection_RecoLanguage);

        /// <summary>
        /// Gets your participant identifier
        /// </summary>
        public string ParticipantId
            => Properties.GetProperty(PropertyId.Conversation_ParticipantId);

        /// <summary>
        /// Gets or sets the authorization token used to connect to the conversation service
        /// </summary>
        public string AuthorizationToken => Properties.GetProperty(PropertyId.SpeechServiceAuthorization_Token);

        /// <summary>
        /// Gets the collection of properties and their values defined for this <see cref="ConversationTranslator"/>.
        /// </summary>
        public PropertyCollection Properties { get; internal set; }

        /// <summary>
        /// Joins an existing conversation. You should use this method if you have created a conversation
        /// using <see cref="Conversation.CreateConversationAsync(SpeechConfig, string)"/>.
        /// </summary>
        /// <param name="conversation">The conversation to join.</param>
        /// <param name="nickname">The display name to use for the current participant.</param>
        /// <returns>An asynchronous operation.</returns>
        public Task JoinConversationAsync(Conversation conversation, string nickname)
        {
            if (conversation == null)
            {
                throw new ArgumentNullException(nameof(conversation));
            }

            return RunAsync((h) =>
            {
                using (var nickHandle = new Utf8StringHandle(nickname))
                {
                    return Internal.ConversationTranslator.conversation_translator_join(
                        h, conversation.NativeHandle, nickHandle);
                }
            });
        }

        /// <summary>
        /// Joins an existing conversation.
        /// </summary>
        /// <param name="conversationId">The unique identifier for the conversation to join.</param>
        /// <param name="nickname">The display name to use for the current participant.</param>
        /// <param name="lang">The speech language to use for the current participant.</param>
        /// <returns>An asynchronous operation.</returns>
        public Task JoinConversationAsync(string conversationId, string nickname, string lang)
        {
            if (string.IsNullOrWhiteSpace(conversationId))
            {
                throw new ArgumentException(nameof(conversationId) + " cannot be null, empty or consist only of white space");
            }
            else if (string.IsNullOrWhiteSpace(lang))
            {
                throw new ArgumentException(nameof(lang) + " cannot be null, empty or consist only of white space");
            }

            return RunAsync((h) =>
            {
                using (var idHandle = new Utf8StringHandle(conversationId))
                using (var nickHandle = new Utf8StringHandle(nickname))
                using (var langHandle = new Utf8StringHandle(lang))
                {
                    return Internal.ConversationTranslator.conversation_translator_join_with_id(
                        h, idHandle, nickHandle, langHandle);
                }
            });
        }

        /// <summary>
        /// Starts sending audio to the conversation service for speech recognition and translation. You
        /// should subscribe to the <see cref="Transcribing"/>, and <see cref="Transcribed"/> events to
        /// receive conversation translation results for yourself, and other participants in the
        /// conversation.
        /// </summary>
        /// <returns>An asynchronous operation.</returns>
        public Task StartTranscribingAsync()
            => RunAsync(Internal.ConversationTranslator.conversation_translator_start_transcribing);

        /// <summary>
        /// Stops sending audio to the conversation service. You will still receive <see cref="Transcribing"/>,
        /// and <see cref="Transcribed"/> events for other participants in the conversation.
        /// </summary>
        /// <returns>An asynchronous operation.</returns>
        public Task StopTranscribingAsync()
            => RunAsync(Internal.ConversationTranslator.conversation_translator_stop_transcribing);

        /// <summary>
        /// Sends an instant message to all participants in the conversation. This instant message
        /// will be translated into each participant's text language.
        /// </summary>
        /// <param name="message">The message to send.</param>
        /// <returns>An asynchronous operation.</returns>
        public Task SendTextMessageAsync(string message)
        {
            if (string.IsNullOrWhiteSpace(message))
            {
                throw new ArgumentException(nameof(message) + " cannot be null, empty or consist only of white space");
            }

            return RunAsync((h) =>
            {
                using (var messageHandle = new Utf8StringHandle(message))
                {
                    return Internal.ConversationTranslator.conversation_translator_send_text_message(
                        h, messageHandle);
                }
            });
        }

        /// <summary>
        /// Sets the Cognitive Speech authorization token that will be used for connecting to the server.
        /// </summary>
        /// <param name="authToken">The authorization token.</param>
        /// <param name="region">(Optional) The Azure region for this token.</param>
        public void SetAuthorizationToken(string authToken, string region = null)
        {
            if (string.IsNullOrWhiteSpace(authToken))
            {
                throw new ArgumentException(nameof(authToken) + " cannot be null, empty or consist only of white space");
            }

            using (var authTokenHandle = new Utf8StringHandle(authToken))
            using (var regionHandle = new Utf8StringHandle(region))
            {
                ThrowIfFail(Internal.ConversationTranslator.conversation_translator_set_authorization_token(_nativeHandle, authTokenHandle, regionHandle));
            }
        }

        /// <summary>
        /// Leave the current conversation. After this is called, you will no longer receive any events.
        /// </summary>
        /// <returns>An asynchronous operation.</returns>
        public Task LeaveConversationAsync()
            => RunAsync(Internal.ConversationTranslator.conversation_translator_leave);

        /// <summary>
        /// Disposes of the object
        /// </summary>
        /// <param name="disposeManaged">True to dispose managed resources</param>
        /// <returns></returns>
        protected override void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                if (_activeAsyncCounter > 0)
                {
                    throw new InvalidOperationException("Cannot dispose a conversation translator while an async action is running. Await async actions to avoid unexpected disposals.");
                }

                _sessionStarted?.Dispose();
                _sessionStopped?.Dispose();
                _canceled?.Dispose();
                _participantsChanged?.Dispose();
                _conversationExpiration?.Dispose();
                _transcribing?.Dispose();
                _transcribed?.Dispose();
                _textMessageReceived?.Dispose();

                // This will make Properties unaccessible.
                Properties.Close();
            }
            _audioInputKeepAlive = null;
            _nativeHandle?.Dispose();
        }

        private static InteropSafeHandle Create(Audio.AudioConfig audioConfig, bool validateConfig)
        {
            if (validateConfig && audioConfig == null)
            {
                throw new ArgumentNullException(nameof(audioConfig));
            }

            IntPtr handle = Internal.ConversationTranslator.SPXHANDLE_INVALID;
            InteropSafeHandle audioConfigHandle = audioConfig == null
                ? new InteropSafeHandle(new IntPtr(-1), null)
                : audioConfig.configHandle;

            ThrowIfFail(Internal.ConversationTranslator.conversation_translator_create_from_config(
                out handle, audioConfigHandle));

            if (audioConfig != null)
            {
                GC.KeepAlive(audioConfig);
            }

            return new InteropSafeHandle(handle, Internal.ConversationTranslator.conversation_translator_handle_release);
        }

        private Task RunAsync(Func<InteropSafeHandle, IntPtr> method, [CallerMemberName]string caller = null, [CallerLineNumber]int line = 0)
        {
            return Task.Run(() =>
            {
                CheckDisposed();
                ThrowIfNull(_nativeHandle);

                try
                {
                    Interlocked.Increment(ref _activeAsyncCounter);
                    ThrowIfFail(method(_nativeHandle));
                }
                catch (Exception ex)
                {
                    string source = string.Format(CultureInfo.InvariantCulture, "[{0}:{1}] {2}", caller, line, method?.Method);

                    // add information to the exception about who called this method, as well the function that failed
                    ex.Source = source;

                    // log to debug output
                    string debugLog = string.Format(CultureInfo.InvariantCulture, "{0} - {1}: {2}", ex.Source, ex.GetType().Name, ex.Message);
                    System.Diagnostics.Debug.WriteLine(debugLog);

                    // finally rethrow the original exception
                    throw;
                }
                finally
                {
                    Interlocked.Decrement(ref _activeAsyncCounter);
                }
            });
        }
    }
}
