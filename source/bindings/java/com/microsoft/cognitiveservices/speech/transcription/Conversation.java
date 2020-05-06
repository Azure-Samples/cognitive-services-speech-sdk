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
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.StringRef;
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
                IntRef handleRef = new IntRef(0);
                Contracts.throwIfFail(createConversationFromConfig(handleRef, finalSpeechConfig.getImpl(), finalConversationId));
                return new Conversation(handleRef.getValue());
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
                IntRef handleRef = new IntRef(0);
                Contracts.throwIfFail(createConversationFromConfig(handleRef, finalSpeechConfig.getImpl(), finalConversationId));
                return new Conversation(handleRef.getValue());
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

        StringRef conversationIdRef = new StringRef("");
        Contracts.throwIfFail(getConversationId(conversationHandle, conversationIdRef));
        return conversationIdRef.getValue();
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
        propertyHandle.setProperty(PropertyId.SpeechServiceAuthorization_Token, token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * The collection of properties and their values defined for this Conversation.
     * @return The collection of properties and their values defined for this Conversation.
     */
    public PropertyCollection getProperties() {
        return this.propertyHandle;
    }

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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(addParticipant(conversationHandle, finalParticipant.getImpl())); }};
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
                Participant participant = Participant.from(userId);
                Contracts.throwIfFail(addParticipant(conversationHandle, participant.getImpl()));
                result[0] = participant;
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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(addParticipantByUser(conversationHandle, user.getImpl())); }};
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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(removeParticipantByUser(conversationHandle, finalUser.getImpl())); }};
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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(removeParticipant(conversationHandle, finalParticipant.getImpl())); }};
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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(removeParticipantByUserId(conversationHandle, finalUserId)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     *   End a conversation.
     *
     * @return An empty task representing the asynchronous operation that ending a conversation.
     */
    public Future<Void> endConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(endConversation(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Start a conversation.
     *
     * @return An asynchronous operation representing starting a conversation.
     */
    public Future<Void> startConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(startConversation(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Delete a conversation. After this no one will be able to join the conversation.
     *
     * @return An asynchronous operation representing deleting a conversation.
     */
    public Future<Void> deleteConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(deleteConversation(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Lock a conversation. This will prevent new participants from joining.
     *
     * @return An asynchronous operation representing locking a conversation.
     */
    public Future<Void> lockConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(lockConversation(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Unlocks a conversation.
     *
     * @return An asynchronous operation representing unlocking a conversation.
     */
    public Future<Void> unlockConversationAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(unlockConversation(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Mute all other participants in the conversation. After this no other participants will have their speech recognitions broadcast, nor be able to send text messages.
     *
     * @return An asynchronous operation representing muting all participants.
     */
    public Future<Void> muteAllParticipantsAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(muteAll(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Unmute all other participants in the conversation.
     *
     * @return An asynchronous operation representing un-muting all participants.
     */
    public Future<Void> unmuteAllParticipantsAsync() {
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(unmuteAll(conversationHandle)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Mute a participant.
     *
     * @param userId A user identifier.
     * @return An asynchronous operation representing muting a particular participant.
     */
    public Future<Void> muteParticipantAsync(String userId) {
        final String finalUserId = userId;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(muteParticipant(conversationHandle, finalUserId)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /**
     * Unmute a participant.
     *
     * @param userId A user identifier.
     * @return An asynchronous operation representing un-muting a particular participant.
     */
    public Future<Void> unmuteParticipantAsync(String userId) {
        final String finalUserId = userId;
        final Conversation thisConv = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(unmuteParticipant(conversationHandle, finalUserId)); }};
                thisConv.doAsyncConversationAction(runnable);
                return null;
        }});
    }

    /*! \cond INTERNAL */

    /**
     * Returns the comversation implementation.
     * @return The implementation of the conversation.
     */
    public SafeHandle getImpl() {
        return conversationHandle;
    }
    
    /*! \endcond */

    /*! \cond PROTECTED */

    protected void dispose(boolean disposing)
    {
        if (disposed) {
            return;
        }

        if (disposing) {
            if (conversationHandle != null) {
                conversationHandle.close();
                conversationHandle = null;
            }

            if (propertyHandle != null) {
                propertyHandle.close();
                propertyHandle = null;
            }
            disposed = true;
        }
    }

    /**
    * Protected constructor.
    * @param conversation Internal conversation implementation
    */
    protected Conversation(long handleValue) {
        this.conversationHandle = new SafeHandle(handleValue, SafeHandleType.Conversation);
        IntRef propbagRef = new IntRef(0);
        Contracts.throwIfFail(getPropertyBag(conversationHandle, propbagRef));
        propertyHandle = new PropertyCollection(propbagRef);
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


    private SafeHandle conversationHandle;
    private boolean disposed = false;
    private final Object conversationLock = new Object();
    private int activeAsyncConversationCounter = 0;
    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

    private final static native long createConversationFromConfig(IntRef convHandle, SafeHandle speechConfigHandle, String id);
    private final native long getConversationId(SafeHandle convHandle, StringRef conversationIdRef);
    private final native long addParticipant(SafeHandle convHandle, SafeHandle participantHandle);
    private final native long addParticipantByUser(SafeHandle convHandle, SafeHandle userHandle);
    private final native long removeParticipant(SafeHandle convHandle, SafeHandle participantHandle);
    private final native long removeParticipantByUser(SafeHandle convHandle, SafeHandle userHandle);
    private final native long removeParticipantByUserId(SafeHandle convHandle, String userId);
    private final native long startConversation(SafeHandle convHandle);
    private final native long endConversation(SafeHandle convHandle);
    private final native long deleteConversation(SafeHandle convHandle);
    private final native long lockConversation(SafeHandle convHandle);
    private final native long unlockConversation(SafeHandle convHandle);
    private final native long muteParticipant(SafeHandle convHandle, String userId);
    private final native long muteAll(SafeHandle convHandle);
    private final native long unmuteParticipant(SafeHandle convHandle, String userId);
    private final native long unmuteAll(SafeHandle convHandle);
    private final native long getPropertyBag(SafeHandle convHandle, IntRef propbagHandle);
}
