package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.internal.NoMatchReason;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.translation.TranslationTextResult;


/**
  * Contains detailed information for NoMatch recognition results.
  */
public class NoMatchDetails {

    private com.microsoft.cognitiveservices.speech.NoMatchReason _reason;
    private com.microsoft.cognitiveservices.speech.internal.NoMatchDetails _noMatchImpl;

    /**
      * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
      * @param The recognition result that was not recognized.
      * @return The no match details object being created.
      */
      public static com.microsoft.cognitiveservices.speech.NoMatchDetails fromResult(SpeechRecognitionResult result) {
        com.microsoft.cognitiveservices.speech.internal.NoMatchDetails noMatchImpl = com.microsoft.cognitiveservices.speech.internal.NoMatchDetails.FromResult(result.getResultImpl());
        return new NoMatchDetails(noMatchImpl);
    }

    /**
      * Creates an instance of NoMatchDetails object for the NoMatch IntentRecognitionResult.
      * @param The recognition result that was not recognized.
      * @return The no match details object being created.
      */
    public static com.microsoft.cognitiveservices.speech.NoMatchDetails fromResult(IntentRecognitionResult result) {
        com.microsoft.cognitiveservices.speech.internal.NoMatchDetails noMatchImpl = com.microsoft.cognitiveservices.speech.internal.NoMatchDetails.FromResult(result.getResultImpl());
        return new NoMatchDetails(noMatchImpl);
    }

    /**
      * Creates an instance of NoMatchDetails object for the NoMatch TranslationTextResult.
      * @param The recognition result that was not recognized.
      * @return The no match details object being created.
      */
    public static com.microsoft.cognitiveservices.speech.NoMatchDetails fromResult(TranslationTextResult result) {
        com.microsoft.cognitiveservices.speech.internal.NoMatchDetails noMatchImpl = com.microsoft.cognitiveservices.speech.internal.NoMatchDetails.FromResult(result.getResultImpl());
        return new NoMatchDetails(noMatchImpl);
    }

    protected NoMatchDetails(com.microsoft.cognitiveservices.speech.internal.NoMatchDetails noMatch) {
        Contracts.throwIfNull(noMatch, "noMatch");

        this._noMatchImpl = noMatch;
        this._reason = com.microsoft.cognitiveservices.speech.NoMatchReason.values()[noMatch.getReason().swigValue() - 1]; // Native NoMatchReason enum starts at 1!!
    }

    /**
      * Explicitly frees any external resource attached to the object
      */
    public void close() {
        if (this._noMatchImpl != null) {
            this._noMatchImpl.delete();
        }
        this._noMatchImpl = null;
    }

    /**
      * The reason the recognition was canceled.
      * @return Specifies the reason canceled.
      */
    public com.microsoft.cognitiveservices.speech.NoMatchReason getReason() {
        return this._reason;
    }

    /**
      * Returns a String that represents the no match details.
      * @return A String that represents the no match details.
      */
    @Override
    public String toString() {
        return "NoMatchReason:" + this._reason;
    }
}
