//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * A conversation translator that enables a connected experience where participants can use their
 * own devices to see everyone else's recognitions and IMs in their own languages. Participants can
 * also speak and send IMs to others. Added in version 1.12.0
 */
public class ConversationTranslator implements Closeable {

    /*! \cond PROTECTED */

    protected static ExecutorService s_executorService;

    protected AtomicInteger eventCounter = new AtomicInteger(0);
    protected Integer backgroundAttempts = 0;
    /*! \endcond */

    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    /** Creates a new instance of the Conversation Translator using the default microphone input. */
    public ConversationTranslator() {
        this(null);
    }

    /**
     * Creates a new instance of the Conversation Translator.
     *
     * @param audioConfig Audio configuration.
     */
    public ConversationTranslator(AudioConfig audioConfig) {
        translatorHandle = new SafeHandle(0, SafeHandleType.ConversationTranslator);
        // Note: Since ais is optional, no test for null reference
        audioInputKeepAlive = audioConfig;
        if (audioConfig == null) {
            Contracts.throwIfFail(createConversationTranslatorFromConfig(translatorHandle, null));
        } else {
            Contracts.throwIfFail(
                    createConversationTranslatorFromConfig(translatorHandle, audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * The collection or properties and their values defined for this ConversationTranslator.
     *
     * @return The collection or properties and their values defined for this
     *     ConversationTranslator.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    /**
     * Gets the language name that is used for recognition.
     *
     * @return The language name that is used for recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Joins an existing conversation. You should use this method if you have created a conversation
     * using CreateConversationAsync(SpeechConfig, String).
     *
     * @param conversation The Conversation to join.
     * @param nickname The display name to use for the current participant.
     * @return A task representing the join operation
     */
    public Future<Void> joinConversationAsync(Conversation conversation, final String nickname) {
        final ConversationTranslator thisReco = this;
        final Conversation thisConv = conversation;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.joinConversation(
                                                translatorHandle, thisConv.getImpl(), nickname));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Joins an existing conversation. You should use this method if you have created a conversation
     * using CreateConversationAsync(SpeechConfig, String).
     *
     * @param conversationId The unique identifier for the conversation to join.
     * @param nickname The display name to use for the current participant.
     * @param language The speech language to use for the current participant.
     * @return A task representing the join operation
     */
    public Future<Void> joinConversationAsync(
            final String conversationId, final String nickname, final String language) {
        final ConversationTranslator thisReco = this;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.joinConversationId(
                                                translatorHandle, conversationId, nickname, language));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Leave the current conversation. After this is called, you will no longer receive any events.
     *
     * @return A task representing the leave operation
     */
    public Future<Void> leaveConversationAsync() {
        final ConversationTranslator thisReco = this;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.leaveConversation(translatorHandle));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Sends an instant message to all participants in the conversation. This instant message will
     * be translated into each participant's text language.
     *
     * @param message The message to send.
     * @return A task representing the send message operation
     */
    public Future<Void> sendTextMessageAsync(final String message) {
        final ConversationTranslator thisReco = this;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.sendText(translatorHandle, message));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Starts sending audio to the conversation service for speech recognition and translation. You
     * should subscribe to the Transcribing, and Transcribed events to receive conversation
     * translation results for yourself, and other participants in the conversation.
     * @return A task representing the transcribe operation
     */
    public Future<Void> startTranscribingAsync() {
        final ConversationTranslator thisReco = this;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.startTranscribing(translatorHandle));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Stops sending audio to the conversation service. You will still receive Transcribing, and
     * Transcribed events for other participants in the conversation.
     * @return A task representing the transcribe operation
     */
    public Future<Void> stopTranscribingAsync() {
        final ConversationTranslator thisReco = this;

        return s_executorService.submit(
                new java.util.concurrent.Callable<Void>() {
                    public Void call() {
                        Runnable runnable =
                                new Runnable() {
                                    public void run() {
                                        Contracts.throwIfFail(thisReco.stopTranscribing(translatorHandle));
                                    }
                                };
                        thisReco.doAsyncRecognitionAction(runnable);
                        return null;
                    }
                });
    }

    /**
     * Event that signals an error with the conversation translation, or the end of the audio
     * stream has been reached.
     */
    public final EventHandlerImpl<ConversationTranslationCanceledEventArgs> canceled =
            new EventHandlerImpl<ConversationTranslationCanceledEventArgs>(eventCounter);

    /** Event that signals how many more minutes are left before the conversation expires. */
    public final EventHandlerImpl<ConversationExpirationEventArgs> conversationExpiration =
            new EventHandlerImpl<ConversationExpirationEventArgs>(eventCounter);

    /** Event that signals participants in the room have changed (e.g. a new participant joined). */
    public final EventHandlerImpl<ConversationParticipantsChangedEventArgs> participantsChanged =
            new EventHandlerImpl<ConversationParticipantsChangedEventArgs>(eventCounter);

    /** Event that signals the start of a conversation translation session. */
    public final EventHandlerImpl<SessionEventArgs> sessionStarted =
            new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /** Event that signals the end of a conversation translation session. */
    public final EventHandlerImpl<SessionEventArgs> sessionStopped =
            new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /** Event that signals a translated text message from a conversation participant. */
    public final EventHandlerImpl<ConversationTranslationEventArgs> textMessageReceived =
            new EventHandlerImpl<ConversationTranslationEventArgs>(eventCounter);

    /**
     * Event that signals a final conversation translation result is available for a conversation
     * participant.
     */
    public final EventHandlerImpl<ConversationTranslationEventArgs> transcribed =
            new EventHandlerImpl<ConversationTranslationEventArgs>(eventCounter);

    /**
     * Event that signals an intermediate conversation translation result is available for a
     * conversation participant.
     */
    public final EventHandlerImpl<ConversationTranslationEventArgs> transcribing =
            new EventHandlerImpl<ConversationTranslationEventArgs>(eventCounter);

    /**
     * Dispose of associated resources. Note: close() must be called in order to relinquish
     * underlying resources held by the object.
     */
    @Override
    public void close() {
        synchronized (translatorLock) {
            if (activeAsyncRecognitionCounter != 0) {
                throw new IllegalStateException(
                        "Cannot dispose a conversationTranslator while async transcription is running. Await async recognitions to avoid unexpected disposals.");
            }
            dispose(true);
        }
    }

    /*! \cond INTERNAL */
    /**
     * Returns the internal conversation translator instance
     * @return The internal conversation translator instance
     */
    public SafeHandle getImpl() {
        return translatorHandle;
    }
    /*! \endcond */

    private void dispose(final boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            if (this.eventCounter.get() != 0 && backgroundAttempts <= 50) {
                // There is an event callback in progress, closing while in an event call results in
                // SWIG problems, so
                // spin a thread to try again in 500ms and return.
                getProperties()
                        .getProperty(
                                "Backgrounding release "
                                        + backgroundAttempts.toString()
                                        + " "
                                        + this.eventCounter.get(),
                                "");
                Thread t =
                        new Thread(
                                new Runnable() {
                                    @Override
                                    public void run() {
                                        try {
                                            Thread.sleep(500 * ++backgroundAttempts);
                                            dispose(disposing);
                                        } catch (Exception e) {
                                        }
                                    }
                                });
                t.start();
            } else {

                if (propertyHandle != null) {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (translatorHandle != null) {
                    translatorHandle.close();
                    translatorHandle = null;
                }
                conversationTranslatorObjects.remove(this);
                audioInputKeepAlive = null;
                disposed = true;
            }
        }
    }

    private void doAsyncRecognitionAction(Runnable recoImplAction) {
        synchronized (translatorLock) {
            activeAsyncRecognitionCounter++;
        }
        if (disposed) {
            throw new IllegalStateException(this.getClass().getName());
        }
        try {
            recoImplAction.run();
        } finally {
            synchronized (translatorLock) {
                activeAsyncRecognitionCounter--;
            }
        }
    }

    private void initialize() {
        final ConversationTranslator _this = this;

        this.sessionStarted.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(
                                sessionStartedSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.sessionStopped.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(
                                sessionStoppedSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.canceled.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(canceledSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.conversationExpiration.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(
                                conversationExpireSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.participantsChanged.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(
                                participantsChangedSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.transcribing.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(transcribingSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.transcribed.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(transcribedSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        this.textMessageReceived.updateNotificationOnConnected(
                new Runnable() {
                    @Override
                    public void run() {
                        conversationTranslatorObjects.add(_this);
                        Contracts.throwIfFail(textMessageSetCallback(_this.translatorHandle.getValue()));
                    }
                });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromHandle(_this.translatorHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void sessionStartedEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStarted;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }
        } catch (Exception ex) {
        }
    }

    private void sessionStoppedEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStopped;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }
        } catch (Exception ex) {
        }
    }

    private void canceledEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationTranslationCanceledEventArgs resultEventArg =
                    new ConversationTranslationCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranslationCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    private void participantsChangedEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationParticipantsChangedEventArgs resultEventArg =
                    new ConversationParticipantsChangedEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationParticipantsChangedEventArgs> handler =
                    this.participantsChanged;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    private void conversationExpireEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationExpirationEventArgs resultEventArg =
                    new ConversationExpirationEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationExpirationEventArgs> handler = this.conversationExpiration;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    private void transcribingEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationTranslationEventArgs resultEventArg =
                    new ConversationTranslationEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranslationEventArgs> handler = this.transcribing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    private void transcribedEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationTranslationEventArgs resultEventArg =
                    new ConversationTranslationEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranslationEventArgs> handler = this.transcribed;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    private void textMessageEventCallback(long eventArgs) {
        try {
            Contracts.throwIfNull(this, "conversationTranslator");
            if (this.disposed) {
                return;
            }
            ConversationTranslationEventArgs resultEventArg =
                    new ConversationTranslationEventArgs(eventArgs, true);
            EventHandlerImpl<ConversationTranslationEventArgs> handler = this.textMessageReceived;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception ex) {
        }
    }

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream
     * events.
     */
    static java.util.Set<ConversationTranslator> conversationTranslatorObjects =
            java.util.Collections.synchronizedSet(new java.util.HashSet<ConversationTranslator>());

    private SafeHandle translatorHandle = null;

    private final native long createConversationTranslatorFromConfig(
            SafeHandle translatorHandle, SafeHandle audioConfigHandle);

    private final native long getPropertyBagFromHandle(SafeHandle translatorHandle, IntRef propertyHandle);

    private final native long sessionStartedSetCallback(long translatorHandle);

    private final native long sessionStoppedSetCallback(long translatorHandle);

    private final native long participantsChangedSetCallback(long translatorHandle);

    private final native long transcribingSetCallback(long translatorHandle);

    private final native long transcribedSetCallback(long translatorHandle);

    private final native long textMessageSetCallback(long translatorHandle);

    private final native long conversationExpireSetCallback(long translatorHandle);

    private final native long canceledSetCallback(long translatorHandle);

    private final native long joinConversation(
            SafeHandle translatorHandle, SafeHandle conversationHandle, String nickname);

    private final native long joinConversationId(
            SafeHandle translatorHandle, String conversationId, String nickname, String language);

    private final native long leaveConversation(SafeHandle translatorHandle);

    private final native long sendText(SafeHandle translatorHandle, String text);

    private final native long startTranscribing(SafeHandle translatorHandle);

    private final native long stopTranscribing(SafeHandle translatorHandle);

    private final Object translatorLock = new Object();
    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
    private int activeAsyncRecognitionCounter = 0;
    private AudioConfig audioInputKeepAlive = null;
}
