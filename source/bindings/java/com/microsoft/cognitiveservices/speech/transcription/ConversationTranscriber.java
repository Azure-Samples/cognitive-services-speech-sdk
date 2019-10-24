//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.OutputFormat;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

/**
 * Performs conversation transcribing for audio input streams, and gets transcribed text and user id as a result.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 * Added in version 1.5.0
 */
public final class ConversationTranscriber extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
     * The event transcribing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> transcribing = new EventHandlerImpl<ConversationTranscriptionEventArgs>();

    /**
     * The event transcribed signals that a final recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> transcribed = new EventHandlerImpl<ConversationTranscriptionEventArgs>();

    /**
     * The event canceled signals that the recognition was canceled.
     */
    final public EventHandlerImpl<ConversationTranscriptionCanceledEventArgs> canceled = new EventHandlerImpl<ConversationTranscriptionCanceledEventArgs>();

    /**
     * Initializes a new instance of Conversation Transcriber.
     */
    public ConversationTranscriber() {
        super(null);

        this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig();
        initialize();
    }

    /**
     * Initializes a new instance of Conversation Transcriber.
     * @param audioConfig audio configuration.
     */
    public ConversationTranscriber(AudioConfig audioConfig) {
        super(audioConfig);

        if (audioConfig == null) {
            this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig();
        } else {
            this.transcriberImpl = com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber.FromConfig(audioConfig.getConfigImpl());
        }
        initialize();
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
     *  Join a conversation.
     *  A conversation transcriber must join a conversation before transcribing audio.
     * @param conversation the conversation to be joined.
     * @return A task representing the asynchronous operation that joins a conversation.
     */
    public Future<Void> joinConversationAsync(Conversation conversation) {
        final ConversationTranscriber thisReco = this;
        final Conversation thisConv = conversation;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { transcriberImpl.JoinConversationAsync(thisConv.getConversationImpl()); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

     /**
     *  Leave a conversation.
     *  After leaving a conversation, no transcribing and transcribed events will be sent out.
     *
     * @return A task representing the asynchronous operation that leaves a conversation.
     */
    public Future<Void> leaveConversationAsync() {
        final ConversationTranscriber thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { transcriberImpl.LeaveConversationAsync(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

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
     *  Stops conversation transcribing.
     *
     *  @returns A task representing the asynchronous operation that stops the transcription.
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

    /*! \cond PROTECTED */

    @Override
    protected void dispose(boolean disposing)
    {
        if (disposed) {
            return;
        }

        if (disposing) {
            if (this.transcribing.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getTranscribing().RemoveEventListener(transcribingHandler);
            if (this.transcribed.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getTranscribed().RemoveEventListener(recognizedHandler);
            if (this.canceled.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getCanceled().RemoveEventListener(errorHandler);
            if (this.sessionStarted.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            if (this.sessionStopped.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            if (this.speechStartDetected.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
            if (this.speechEndDetected.isUpdateNotificationOnConnectedFired())
                transcriberImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

            transcribingHandler.delete();
            recognizedHandler.delete();
            errorHandler.delete();
            transcriberImpl.delete();
            _Parameters.close();

            _conversationTranscriberObjects.remove(this);
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

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<ConversationTranscriber> _conversationTranscriberObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<ConversationTranscriber>());

    /*! \endcond */

    private void initialize() {
        super.internalRecognizerImpl = this.transcriberImpl;

        final ConversationTranscriber _this = this;

        transcribingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        this.transcribing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getTranscribing().AddEventListener(transcribingHandler);
            }
        });

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        this.transcribed.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getTranscribed().AddEventListener(recognizedHandler);
            }
        });

        errorHandler = new CanceledHandlerImpl(this);
        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getCanceled().AddEventListener(errorHandler);
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _conversationTranscriberObjects.add(_this);
                transcriberImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);
            }
        });

        _Parameters = new PrivatePropertyCollection(transcriberImpl.getProperties());
    }

    private com.microsoft.cognitiveservices.speech.internal.ConversationTranscriber transcriberImpl;
    private ResultHandlerImpl transcribingHandler;
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

        ResultHandlerImpl(ConversationTranscriber transcriber, boolean isTranscribedHandler) {
            Contracts.throwIfNull(transcriber, "transcriber");

            this.transcriber = transcriber;
            this.isRecognizedHandler = isTranscribedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.ConversationTranscriptionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (transcriber.disposed) {
                return;
            }

            ConversationTranscriptionEventArgs resultEventArg = new ConversationTranscriptionEventArgs(eventArgs);
            EventHandlerImpl<ConversationTranscriptionEventArgs> handler = isRecognizedHandler ? transcriber.transcribed : transcriber.transcribing;
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
