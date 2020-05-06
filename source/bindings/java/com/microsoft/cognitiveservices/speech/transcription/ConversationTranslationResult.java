//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//
package com.microsoft.cognitiveservices.speech.transcription;

import com.microsoft.cognitiveservices.speech.translation.TranslationRecognitionResult;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.StringRef;

/** Defines a conversation translation result. Added in 1.12.0. */
public class ConversationTranslationResult extends TranslationRecognitionResult {

    ConversationTranslationResult(long result) {
        super(result);
        Contracts.throwIfNull(resultHandle, "resultHandle");

        StringRef stringVal = new StringRef("");
        Contracts.throwIfFail(getOriginalLang(resultHandle, stringVal));
        this.originalLang = stringVal.getValue();

        Contracts.throwIfFail(getParticipantId(resultHandle, stringVal));
        this.participantId = stringVal.getValue();
    }

    /**
     * The original language this result was in.
     * @return The original language this result was in.
     */
    public String getOriginalLang() {
        return this.originalLang;
    }

    /**
     * The unique identifier for the participant this result is for.
     * @return The unique identifier for the participant this result is for.
     */
    public String getParticipantId() {
        return this.participantId;
    }

    private String originalLang;
    private String participantId;

    private final native long getOriginalLang(SafeHandle resultHandle, StringRef originalLang);

    private final native long getParticipantId(SafeHandle resultHandle, StringRef participantId);
}
