//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
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

    private SafeHandle participantHandle = null;
    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

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
}
