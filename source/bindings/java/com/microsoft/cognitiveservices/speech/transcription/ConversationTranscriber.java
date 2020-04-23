//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.Future;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriptionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
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
    private AtomicInteger eventCounter = new AtomicInteger(0);

    /**
     * The event transcribing signals that an intermediate recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> transcribing = new EventHandlerImpl<ConversationTranscriptionEventArgs>(eventCounter);

    /**
     * The event transcribed signals that a final recognition result is received.
     */
    final public EventHandlerImpl<ConversationTranscriptionEventArgs> transcribed = new EventHandlerImpl<ConversationTranscriptionEventArgs>(eventCounter);

    /**
     * The event canceled signals that the recognition was canceled.
     */
    final public EventHandlerImpl<ConversationTranscriptionCanceledEventArgs> canceled = new EventHandlerImpl<ConversationTranscriptionCanceledEventArgs>(eventCounter);

    /**
     * Initializes a new instance of Conversation Transcriber.
     */
    public ConversationTranscriber() {
        super(null);

        Contracts.throwIfNull(recoHandle, "recoHandle");
        Contracts.throwIfFail(createConversationTranscriberFromConfig(recoHandle, null));
        initialize();
    }

    /**
     * Initializes a new instance of Conversation Transcriber.
     * @param audioConfig audio configuration.
     */
    public ConversationTranscriber(AudioConfig audioConfig) {
        super(audioConfig);

        if (audioConfig == null) {
            Contracts.throwIfFail(createConversationTranscriberFromConfig(recoHandle, null));
        } else {
            Contracts.throwIfFail(createConversationTranscriberFromConfig(recoHandle, audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public OutputFormat getOutputFormat() {
        if (propertyHandle.getProperty(PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse).equals("true")) {
            return OutputFormat.Detailed;
        } else {
            return OutputFormat.Simple;
        }
    }

    /**
     * The collection of properties and their values defined for this ConversationTranscriber.
     * @return The collection of properties and their values defined for this ConversationTranscriber.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }


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
                Runnable runnable = new Runnable() { public void run() { thisReco.joinConversation(recoHandle, thisConv.getImpl()); }};
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
                Runnable runnable = new Runnable() { public void run() { thisReco.leaveConversation(recoHandle); }};
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
                Runnable runnable = new Runnable() { public void run() { thisReco.startContinuousRecognition(recoHandle); }};
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
                Runnable runnable = new Runnable() { public void run() { thisReco.stopContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(final boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            if(this.eventCounter.get() != 0 && backgroundAttempts <= 50 ) {
                // There is an event callback in progress, closing while in an event call results in SWIG problems, so 
                // spin a thread to try again in 500ms and return.
                getProperties().getProperty("Backgrounding release " + backgroundAttempts.toString() + " " + this.eventCounter.get(), ""); 
                Thread t = new Thread(
                    new Runnable(){
                        @Override
                        public void run() {
                            try{
                                Thread.sleep(500 * ++backgroundAttempts);
                                dispose(disposing);
                            } catch (Exception e){}
                        }
                    });
                t.start();
            } 
            else {                
                if (propertyHandle != null)
                {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (recoHandle != null)
                {
                    recoHandle.close();
                    recoHandle = null;
                }
                conversationTranscriberObjects.remove(this);
                disposed = true;
                super.dispose(disposing);
            }
        }
    }

    /*! \endcond */

    /*! \cond INTERNAL */

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<ConversationTranscriber> conversationTranscriberObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<ConversationTranscriber>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public SafeHandle getTranscriberImpl() {
        return recoHandle;
    }

    /*! \endcond */

    private void initialize() {
        final ConversationTranscriber _this = this;

        this.transcribing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(recognizingSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.transcribed.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(sessionStartedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(sessionStoppedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(speechStartDetectedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                conversationTranscriberObjects.add(_this);
                Contracts.throwIfFail(speechEndDetectedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromRecognizerHandle(_this.recoHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void recognizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            ConversationTranscriptionEventArgs resultEventArg = new ConversationTranscriptionEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranscriptionEventArgs> handler = this.transcribing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void recognizedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            ConversationTranscriptionEventArgs resultEventArg = new ConversationTranscriptionEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranscriptionEventArgs> handler = this.transcribed;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void canceledEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            ConversationTranscriptionCanceledEventArgs resultEventArg = new ConversationTranscriptionCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranscriptionCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private final native long createConversationTranscriberFromConfig(SafeHandle recoHandle, SafeHandle audioConfigHandle);
    private final native long joinConversation(SafeHandle recoHandle, SafeHandle conversationHandle);
    private final native long leaveConversation(SafeHandle recoHandle);
    
    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
}
