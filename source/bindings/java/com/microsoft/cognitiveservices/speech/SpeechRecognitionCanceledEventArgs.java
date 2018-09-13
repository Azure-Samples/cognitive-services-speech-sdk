package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.internal.CancellationDetails;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;

/**
  * Define payload of speech recognition canceled result events.
  */
public final class SpeechRecognitionCanceledEventArgs {

    SpeechRecognitionCanceledEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs e) {
        Contracts.throwIfNull(e, "e");

        this._eventArgImpl = e;
        this._Result = new SpeechRecognitionResult(e.GetResult());

        this._SessionId = e.getSessionId();
        Contracts.throwIfNull(this._SessionId, "SessionId");

        com.microsoft.cognitiveservices.speech.internal.CancellationDetails cancellation = e.GetCancellationDetails();
        this._cancellationReason  = com.microsoft.cognitiveservices.speech.CancellationReason.values()[cancellation.getReason().swigValue() - 1]; // Native CancellationReason enum starts at 1!!
        this._errorDetails = cancellation.getErrorDetails();
    }

    /**
      * Specifies the recognition result.
      * @return the recognition result.
      */
    public SpeechRecognitionResult getResult() {
        return _Result;
    }

    /**
      * Specifies the session identifier.
      * @return the session identifier.
      */
    public final String getSessionId() {
        return _SessionId;
    }

    /**
      * The reason the recognition was canceled.
      * @return Specifies the reason canceled.
      */
      public CancellationReason getReason() {
        return this._cancellationReason ;
    }

    /**
      * In case of an unsuccessful recognition, provides a details of why the occurred error.
      * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
      * @return A String that represents the error details.
      */
    public String getErrorDetails() {
        return this._errorDetails;
    }

    /**
      * Returns a String that represents the speech recognition canceled event args.
      * @return A String that represents the speech recognition canceled event args.
      */
    @Override
    public String toString() {
        return "SessionId:" + _SessionId +
               " ResultId:" + _Result.getResultId() +
               " CancellationReason:" + _cancellationReason  +
               " Recognized text:<" + _errorDetails +
               ">.";
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs _eventArgImpl;
    private String _SessionId;
    private SpeechRecognitionResult _Result;
    private CancellationReason _cancellationReason ;
    private String _errorDetails;
}
