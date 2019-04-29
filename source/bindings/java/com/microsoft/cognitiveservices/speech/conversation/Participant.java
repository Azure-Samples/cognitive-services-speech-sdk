//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.conversation;

import com.microsoft.cognitiveservices.speech.util.Contracts;
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
        return new Participant(com.microsoft.cognitiveservices.speech.internal.Participant.From(userId, preferredLanguage, voiceSignature));
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
        return new Participant(com.microsoft.cognitiveservices.speech.internal.Participant.From(userId, preferredLanguage));
    }

    /**
     * Create a Participant using user id.
     * @param userId user's Id.
     * @return Participant object.
     */
    public static Participant from(String userId) {
        Contracts.throwIfNullOrWhitespace(userId, "userId");
        return new Participant(com.microsoft.cognitiveservices.speech.internal.Participant.From(userId));
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._participantImpl != null) {
            this._participantImpl.delete();
        }
        this._participantImpl = null;

        if (this._parameters != null) {
            this._parameters.close();
        }
        this._parameters = null;
    }

    Participant(com.microsoft.cognitiveservices.speech.internal.Participant participant) {
        Contracts.throwIfNull(participant, "participant");
        this._participantImpl = participant;
        _parameters = new PrivatePropertyCollection(_participantImpl.getProperties());
    }

    /**
     * Sets the preferred languages for participant.
     * @param preferredLanguage participant's preferred language.
     */
    public void setPreferredLanguage(String preferredLanguage) {
        Contracts.throwIfNullOrWhitespace(preferredLanguage, "preferredLanguage");
        _participantImpl.SetPreferredLanguage(preferredLanguage);
    }

    /**
     * Sets the voice signature for participant.
     * If voice signature is empty then user will not be identified.
     * @param voiceSignature user's voice signature.
     */
    public void setVoiceSignature(String voiceSignature) {
        Contracts.throwIfNull(voiceSignature, "voiceSignature");
        _participantImpl.SetVoiceSignature(voiceSignature);
    }

    /**
     * Gets the properties for participant.
     * @return the properties of participant.
     */
    public PropertyCollection getProperties() {
        return _parameters;
    }

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    private com.microsoft.cognitiveservices.speech.internal.Participant _participantImpl;
    private com.microsoft.cognitiveservices.speech.PropertyCollection _parameters;

    /*! \cond INTERNAL */

    /**
     * Returns the participant implementation.
     * @return The implementation of the participant.
     */
    public com.microsoft.cognitiveservices.speech.internal.Participant getParticipantImpl() {
        return this._participantImpl;
    }

    /*! \endcond */
}
