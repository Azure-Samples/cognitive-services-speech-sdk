//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

/**
 * Represents a participant in a conversation transcribing session.
 * Added in version 1.5.0
 */
public final class Participant
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

     /**
     * Create a Participant using user id, her/his preferred language and her/his voice signature.
     * If voice signature is empty then user will not be identified.
     * @param userId user's Id.
     * @param preferredLanguage users' preferred language.
     * @param voiceSignature user's voice signature.
     * @return Participant object.
     */
    public static Participant from(String userId, String preferredLanguage, String voiceSignature) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        Contracts.throwIfNullOrWhitespace(preferredLanguage, "preferredLanguage");
        Contracts.throwIfNull(voiceSignature, "voiceSignature");

        IntRef participantRef = new IntRef(0);
        Contracts.throwIfFail(createParticipantHandle(participantRef, userId, preferredLanguage, voiceSignature));
        return new Participant(participantRef.getValue());
    }

    /**
     * Create a Participant using user id and her/his preferred language.
     * @param userId user's Id.
     * @param preferredLanguage users' preferred language.
     * @return Participant object.
     */
    public static Participant from(String userId, String preferredLanguage) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        Contracts.throwIfNullOrWhitespace(preferredLanguage, "preferredLanguage");
        IntRef participantRef = new IntRef(0);
        Contracts.throwIfFail(createParticipantHandle(participantRef, userId, preferredLanguage, null));
        return new Participant(participantRef.getValue());
    }

    /**
     * Create a Participant using user id.
     * @param userId user's Id.
     * @return Participant object.
     */
    public static Participant from(String userId) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        IntRef participantRef = new IntRef(0);
        Contracts.throwIfFail(createParticipantHandle(participantRef, userId, null, null));
        return new Participant(participantRef.getValue());
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.participantHandle != null) {
            this.participantHandle.close();
        }
        this.participantHandle = null;

        if (this.propertyHandle != null) {
            this.propertyHandle.close();
        }
        this.propertyHandle = null;
    }

    Participant(long handleValue) {
        this.participantHandle = new SafeHandle(handleValue, SafeHandleType.Participant);
        IntRef propbagRef = new IntRef(0);
        Contracts.throwIfFail(getPropertyBag(participantHandle, propbagRef));
        propertyHandle = new PropertyCollection(propbagRef);

        StringRef stringRef;
        stringRef = new StringRef("");
        Contracts.logErrorIfFail(getUserId(participantHandle, stringRef));
        this.userId = stringRef.getValue();

        stringRef = new StringRef("");
        Contracts.logErrorIfFail(getDisplayName(participantHandle, stringRef));
        this.displayName = stringRef.getValue();

        stringRef = new StringRef("");
        Contracts.logErrorIfFail(getAvatar(participantHandle, stringRef));
        this.avatar = stringRef.getValue();

        isHost = getIsHost(participantHandle);
        isMuted = getIsMuted(participantHandle);
        isUsingTts = getIsUsingTts(participantHandle);
    }

    /**
     * Sets the preferred languages for participant.
     * @param preferredLanguage participant's preferred language.
     */
    public void setPreferredLanguage(String preferredLanguage) {
        Contracts.throwIfNullOrWhitespace(preferredLanguage, "preferredLanguage");
        Contracts.throwIfFail(setPreferredLanguage(participantHandle, preferredLanguage));
    }

    /**
     * Sets the voice signature for participant.
     * If voice signature is empty then user will not be identified.
     * @param voiceSignature user's voice signature.
     */
    public void setVoiceSignature(String voiceSignature) {
        Contracts.throwIfNull(voiceSignature, "voiceSignature");
        Contracts.throwIfFail(setVoiceSignature(participantHandle, voiceSignature));
    }

    /**
     * Gets the properties for participant.
     * @return the properties of participant.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    /**
     * The unique identifier for the participant.
     * @return unique ID
     */
    public String getId() {
        return userId;
    }

    /**
     * Gets the colour of the user's avatar as an HTML hex string (e.g. FF0000 for red).
     * @return hex color string
     */
    public String getAvatar() {
      return avatar;
    }

    /**
     * The participant's display name. Please note that each participant within the same conversation must have a different display name. Duplicate names within the same conversation are not allowed. You can use the Id property as another way to refer to each participant.
     * @return The participant's display name.
     */
    public String getDisplayName() {
        return displayName;
    }

    /**
     * Gets whether or not this participant is the host.
     * @return whether or not this participant is the host.
     */
    public boolean isHost() {
        return isHost;
    }

    /**
     * Gets whether or not this participant is muted.
     * @return whether or not this participant is muted.
     */
    public boolean isMuted() {
        return isMuted;
    }

    /**
     * Gets whether or not the participant is using Text To Speech (TTS).
     * @return whether or not the participant is using Text To Speech (TTS).
     */
    public boolean isUsingTts() {
        return isUsingTts;
    }

    private SafeHandle participantHandle = null;
    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

    private String avatar;
    private String displayName;
    private String voiceSignature;
    private String userId;
    private boolean isHost;
    private boolean isMuted;
    private boolean isUsingTts;

    /*! \cond INTERNAL */

    /**
     * Returns the participant implementation.
     * @return The implementation of the participant.
     */
    public SafeHandle getImpl() {
        return this.participantHandle;
    }

    /*! \endcond */

    private final static native long createParticipantHandle(IntRef participantRef, String userId, String preferredLanguage, String voiceSignature);
    private final native long getPropertyBag(SafeHandle handle, IntRef propbagHandle);
    private final native long setPreferredLanguage(SafeHandle handle, String preferredLanguage);
    private final native long setVoiceSignature(SafeHandle handle, String voiceSignature);
    private final native long getUserId(SafeHandle handle, StringRef propertyRef);
    private final native long getDisplayName(SafeHandle handle, StringRef propertyRef);
    private final native long getAvatar(SafeHandle handle, StringRef propertyRef);
    private final native boolean getIsMuted(SafeHandle handle);
    private final native boolean getIsHost(SafeHandle handle);
    private final native boolean getIsUsingTts(SafeHandle handle);

}
