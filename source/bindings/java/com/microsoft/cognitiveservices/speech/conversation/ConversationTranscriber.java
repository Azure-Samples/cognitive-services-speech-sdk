//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.conversation;

import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.conversation.ConversationTranscriptionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.conversation.Participant;
import com.microsoft.cognitiveservices.speech.conversation.User;

/**
 * Performs conversation transcribing for audio input streams, and gets transcribed text and user id as a result.
 * Added in version 1.5.0
 */
public final class ConversationTranscriber extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> recognizing = new EventHandlerImpl<ConversationTranscriptionEventArgs>();

    /**
     * The event recognized signals that a final recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> recognized = new EventHandlerImpl<ConversationTranscriptionEventArgs>();

    /**
     * The event canceled signals that the recognition was canceled.
     */
    final public EventHandlerImpl<ConversationTranscriptionCanceledEventArgs> canceled = new EventHandlerImpl<ConversationTranscriptionCanceledEventArgs>();

    /**
     * Initializes a new instance of Conversation Transcriber.
     * @param speechConfig speech configuration.
     */
    public ConversationTranscriber(SpeechConfig speechConfig) {
        super(null);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig(speechConfig.getImpl());
        initialize();
    }

    /**
     * Initializes a new instance of Conversation Transcriber.
     * @param speechConfig speech configuration.
     * @param audioConfig audio configuration.
     */
    public ConversationTranscriber(SpeechConfig speechConfig, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(speechConfig, "speechConfig");
        if (audioConfig == null) {
            this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig(speechConfig.getImpl());
        } else {
            this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig(speechConfig.getImpl(), audioConfig.getConfigImpl());
        }
        initialize();
    }

    /**
     * Gets the conversation ID of a conversation transcriber session.
     * @return the conversation ID of a conversation transcribing session.
     */
    public String getConversationId() {
        return transcriberImpl.GetConversationId();
    }

    /**
     * Sets the conversation ID for a conversation transcriber session.
     * @param conversationId conversation id.
     */
    public void setConversationId(String conversationId) {
        Contracts.throwIfNullOrWhitespace(conversationId, "conversationId");
        transcriberImpl.SetConversationId(conversationId);
    }

    /**
     * Add a participant to conversation using userId.
     * @param userId user id.
     * @return Participant object.
     */
    public Participant addParticipant(String userId) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        return new Participant(transcriberImpl.AddParticipant(userId));
    }

    /**
     * Add a participant to conversation using User object.
     * @param user user object.
     */
    public void addParticipant(User user) {
        Contracts.throwIfNull(user, "user");
        transcriberImpl.AddParticipant(user.getUserImpl());
    }

    /**
     * Add a participant to conversation using Participant object.
     * @param participant participant object.
     */
    public void addParticipant(Participant participant) {
        Contracts.throwIfNull(participant, "participant");
        transcriberImpl.AddParticipant(participant.getParticipantImpl());
    }

    /**
     * Remove a participant from conversation using Participant object.
     * @param participant participant object.
     */
    public void removeParticipant(Participant participant) {
        Contracts.throwIfNull(participant, "participant");
        transcriberImpl.RemoveParticipant(participant.getParticipantImpl());
    }

    /**
     * Remove a participant from conversation using User object.
     * @param user user object.
     */
    public void removeParticipant(User user) {
        Contracts.throwIfNull(user, "user");
        transcriberImpl.RemoveParticipant(user.getUserImpl());
    }

    /**
     * Remove a participant from conversation using User id.
     * @param userId user's id.
     */
    public void removeParticipant(String userId) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        transcriberImpl.RemoveParticipant(userId);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
     * the caller needs to refresh it by calling this setter with a new valid token.
     * Otherwise, the recognizer will encounter errors during recognition.
     * @param token Authorization token.
     */
    public void setAuthorizationToken(String token) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        transcriberImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return transcriberImpl.GetAuthorizationToken();
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public OutputFormat getOutputFormat() {
        if (_Parameters.getProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse).equals("true")) {
            return OutputFormat.Detailed;
        } else {
            return OutputFormat.Simple;
        }
    }

    /**
     * The collection of properties and their values defined for this ConversationTranscriber.
     * @return The collection or properties and their values defined for this ConversationTranscriber.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private com.microsoft.cognitiveservices.speech.PropertyCollection _Parameters;

    /**
     * Starts conversation transcribing on a continuous audio stream, until stopTranscribingAsync() is called.
     * User must subscribe to events to receive transcription results.
     * @return A task representing the asynchronous operation that starts the transcription.
     */
    public Future<Void> startTranscribingAsync() {
        final ConversationTranscriber thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { transcriberImpl.StartTranscribingAsync(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Stops conversation transcribing.
     * @return A task representing the asynchronous operation that stops the transcription.
     */
    public Future<Void> stopTranscribingAsync() {
        final ConversationTranscriber thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { transcriberImpl.StopTranscribingAsync(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Ends conversation session.
     * @return A task representing the asynchronous operation that ends the conversation.
     */
    public Future<Void> endConversationAsync() {
        final ConversationTranscriber thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { transcriberImpl.EndConversationAsync(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(boolean disposing)
    {
        if (disposed) {
            return;
        }

        if (disposing) {
            transcriberImpl.getRecognizing().RemoveEventListener(recognizingHandler);
            transcriberImpl.getRecognized().RemoveEventListener(recognizedHandler);
            transcriberImpl.getCanceled().RemoveEventListener(errorHandler);
            transcriberImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            transcriberImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            transcriberImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
            transcriberImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

            recognizingHandler.delete();
            recognizedHandler.delete();
            errorHandler.delete();
            transcriberImpl.delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    /*! \endcond */

    /*! \cond INTERNAL */

    // TODO Remove this... After tests are updated to no longer depend upon this
    public com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber getTranscriberImpl() {
        return transcriberImpl;
    }

    /*! \endcond */

    private void initialize() {
        super.internalRecognizerImpl = this.transcriberImpl;

        recognizingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        transcriberImpl.getRecognizing().AddEventListener(recognizingHandler);

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        transcriberImpl.getRecognized().AddEventListener(recognizedHandler);

        errorHandler = new CanceledHandlerImpl(this);
        transcriberImpl.getCanceled().AddEventListener(errorHandler);

        transcriberImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        transcriberImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        transcriberImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        transcriberImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);

        _Parameters = new PrivatePropertyCollection(transcriberImpl.getProperties());
    }

    private com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber transcriberImpl;
    private ResultHandlerImpl recognizingHandler;
    private ResultHandlerImpl recognizedHandler;
    private CanceledHandlerImpl errorHandler;
    private boolean disposed = false;

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    // Defines a private class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.ConversationTranscriberEventListener {

        ResultHandlerImpl(ConversationTranscriber transcriber, boolean isRecognizedHandler) {
            Contracts.throwIfNull(transcriber, "transcriber");

            this.transcriber = transcriber;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (transcriber.disposed) {
                return;
            }

            ConversationTranscriptionEventArgs resultEventArg = new ConversationTranscriptionEventArgs(eventArgs);
            EventHandlerImpl<ConversationTranscriptionEventArgs> handler = isRecognizedHandler ? transcriber.recognized : transcriber.recognizing;
            if (handler != null) {
                handler.fireEvent(this.transcriber, resultEventArg);
            }
        }

        private ConversationTranscriber transcriber;
        private boolean isRecognizedHandler;
    }

    // Defines a private class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.ConversationTranscriberCanceledEventListener {

        CanceledHandlerImpl(ConversationTranscriber transcriber) {
            Contracts.throwIfNull(transcriber, "transcriber");
            this.transcriber = transcriber;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (transcriber.disposed) {
                return;
            }

            ConversationTranscriptionCanceledEventArgs resultEventArg = new ConversationTranscriptionCanceledEventArgs(eventArgs);
            EventHandlerImpl<ConversationTranscriptionCanceledEventArgs> handler = this.transcriber.canceled;

            if (handler != null) {
                handler.fireEvent(this.transcriber, resultEventArg);
            }
        }

        private ConversationTranscriber transcriber;
    }

}
