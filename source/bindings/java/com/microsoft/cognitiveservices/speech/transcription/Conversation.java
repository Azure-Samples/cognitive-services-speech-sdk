//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import java.io.Closeable;
import java.util.concurrent.Future;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.transcription.Participant;
import com.microsoft.cognitiveservices.speech.transcription.User;

/**
 * Performs conversation management including add and remove participants.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 * Added in version 1.8.0
 */
public final class Conversation implements Closeable
{
    private static ExecutorService s_executorService;

    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    /**
     * Initializes a new instance of Conversation.
     * @param speechConfig speech configuration.
     * @return A task representing the asynchronous operation that creates a conversation.
     */
    public static Future<Conversation> createConversationAsync(SpeechConfig speechConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");
        final String finalConversationId = "";
        final SpeechConfig finalSpeechConfig = speechConfig;

        return s_executorService.submit(new java.util.concurrent.Callable<Conversation>() {
            public Conversation call() {
                com.microsoft.cognitiveservices.speech.internal.Conversation impl = 
                    com.microsoft.cognitiveservices.speech.internal.Conversation.CreateConversationAsync(finalSpeechConfig.getImpl(), finalConversationId)
                    .Get();
                    
                return new Conversation(impl);
            }
        });
    }
    
    /**
     * Initializes a new instance of Conversation.
     * @param speechConfig speech configuration.
     * @param conversationId a unqiue identification of your conversation.
     * @return A task representing the asynchronous operation that creates a conversation.
     */
    public static Future<Conversation> createConversationAsync(SpeechConfig speechConfig, String conversationId) {
        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(conversationId, "conversationId");
        
        final String finalConversationId = conversationId;
        final SpeechConfig finalSpeechConfig = speechConfig;

        return s_executorService.submit(new java.util.concurrent.Callable<Conversation>() {
            public Conversation call() {
                com.microsoft.cognitiveservices.speech.internal.Conversation impl = 
                    com.microsoft.cognitiveservices.speech.internal.Conversation.CreateConversationAsync(finalSpeechConfig.getImpl(), finalConversationId)
                    .Get();
                    
                return new Conversation(impl);
            }
        });
    }

    /**
     * Dispose of associated resources.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        synchronized (conversationLock) {
            if (activeAsyncConversationCounter != 0) {
                throw new IllegalStateException("Cannot dispose a recognizer while async recognition is running. Await async recognitions to avoid unexpected disposals.");
            }
            dispose(true);
        }
    }

    /**
     * Gets the conversation Id.
     * @return the conversation Id.
     */
    public String getConversationId() {
        return conversationImpl.GetConversationId();
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
        conversationImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return conversationImpl.GetAuthorizationToken();
    }

    /**
     * The collection of properties and their values defined for this Conversation.
     * @return The collection or properties and their values defined for this Conversation.
     */
    public PropertyCollection getProperties() {
        return this.parameters;
    }

    private com.microsoft.cognitiveservices.speech.PropertyCollection _Parameters;

    /**
     *   Add a participant to a conversation using a participant object.
     *
     * @param participant A participant object.
     * @return A task representing the asynchronous operation of adding a participant.
     */
    public Future<Participant> addParticipantAsync(Participant participant) {
        final Participant finalParticipant = participant;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Participant>() {
            public Participant call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.AddParticipantAsync(finalParticipant.getParticipantImpl()).Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return finalParticipant;
        }});
    }

    /**
     *   Add a participant to a conversation using the user's id.
     *
     * @param userId The user id.
     * @return A task representing the asynchronous operation of adding a participant.
     */
    public Future<Participant> addParticipantAsync(final String userId) {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Participant>() {
            public Participant call() {
                final Participant[] result = new Participant[1];
                Runnable runnable = new Runnable() { public void run() {
                result[0] = new Participant(conversationImpl.AddParticipantAsync(userId).Get());
                }};
                thisConv.doAsyncConversationAction(runnable);
                return result[0];
        }});
    }

    /**
     *   Add a participant to a conversation using a user object.
     *
     * @param user A user object.
     * @return A task representing the asynchronous operation of adding a participant.
     */
    public Future<User> addParticipantAsync(final User user) {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<User>() {
            public User call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.AddParticipantAsync(user.getUserImpl()).Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return user;
        }});
    }

    /**
     *   Remove a participant from a conversation using a user object.
     *
     * @param user A user object.
     * @return Am empty task representing the asynchronous operation of removing a participant.
     */
    public Future<Void> removeParticipantAsync(User user) {
        final User finalUser = user;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.RemoveParticipantAsync(finalUser.getUserImpl()).Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     *   Remove a participant from a conversation using a participant object.
     *
     * @param participant A participant object.
     * @return Am empty task representing the asynchronous operation of removing a participant.
     */
    public Future<Void> removeParticipantAsync(Participant participant) {
        final Participant finalParticipant = participant;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.RemoveParticipantAsync(finalParticipant.getParticipantImpl()).Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     *   Remove a participant from a conversation using a user id.
     *
     * @param userId A user id.
     * @return Am empty task representing the asynchronous operation of removing a participant.
     */
    public Future<Void> removeParticipantAsync(String userId) {
        final String finalUserId = userId;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.RemoveParticipantAsync(finalUserId).Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     *   End a conversation.
     *
     * @return Am empty task representing the asynchronous operation that ending a conversation.
     */
    public Future<Void> endConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { conversationImpl.EndConversationAsync().Get(); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Returns the comversation implementation.
     * @return The implementation of the conversation.
     */
    public com.microsoft.cognitiveservices.speech.internal.Conversation getConversationImpl() {
        return conversationImpl;
    }

    /*! \cond PROTECTED */

    protected void dispose(boolean disposing)
    {
        if (disposed) {
            return;
        }

        if (disposing) {
            conversationImpl.delete();
            parameters.close();
            disposed = true;
        }
    }

    /**
    * Protected constructor.
    * @param conversation Internal conversation implementation
    */
    protected Conversation(com.microsoft.cognitiveservices.speech.internal.Conversation conversation) {
        Contracts.throwIfNull(conversation, "conversation");
        this.conversationImpl = conversation;
        parameters = new PrivatePropertyCollection(conversationImpl.getProperties());
    }

    /*! \endcond */

    private void doAsyncConversationAction(Runnable conversationImplAction) {
        synchronized (conversationLock) {
            activeAsyncConversationCounter++;
        }
        if (disposed) {
            throw new IllegalStateException(this.getClass().getName());
        }
        try {
            conversationImplAction.run();
        } finally {
            synchronized (conversationLock) {
                activeAsyncConversationCounter--;
            }
        }
    }


    private com.microsoft.cognitiveservices.speech.internal.Conversation conversationImpl;
    private boolean disposed = false;
    private final Object conversationLock = new Object();
    private int activeAsyncConversationCounter = 0;
    private com.microsoft.cognitiveservices.speech.PropertyCollection parameters;

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    private void initialize() {
       this.parameters = new PrivatePropertyCollection(this.conversationImpl.getProperties());
    }
}
