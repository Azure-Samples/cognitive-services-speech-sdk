package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ResultParameterNames;
import com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyValueCollection;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Defines content of an intent intermediate/final result events.
  */
public final class IntentRecognitionResultEventArgs // : System.EventArgs
{
    IntentRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArg)
    {
        Contracts.throwIfNull(eventArg, "eventArg");

        this.eventArgImpl = eventArg;
        this._Result = new IntentRecognitionResult(eventArg.getResult());
        this._SessionId = eventArg.getSessionId();

        Contracts.throwIfNull(this._SessionId, "SessionId");
    }

    /**
      * Represents the intent recognition result.
      * @return Represents the intent recognition result.
      */
    public final IntentRecognitionResult getResult()
    {
        return _Result;
    }//{ get; }
    private IntentRecognitionResult  _Result;

    /**
      * A String represents the session identifier.
      * @return A String represents the session identifier.
      */
    public final String getSessionId()
    {
        return _SessionId;
    }
    // { get; }
    private String _SessionId;
    
    public ResultPropertyValueCollection getProperties()
    {
        ResultPropertyValueCollection r = eventArgImpl.getResult().getProperties();

        Contracts.throwIfNull(r, "result properties");

        return r;
    }

    /**
      * Returns a String that represents the session id and the intent recognition result event.
      * @return A String that represents the intent recognition result event.
      */
    @Override
    public String toString()
    {
        return "SessionId:" + _SessionId +
               " ResultId:" + _Result.getResultId() +
               " Status:" + _Result.getReason() +
               " IntentId:<" + _Result.getIntentId() +
               "> Recognized text:<" + _Result.getText() +
               "> Recognized json:<" + getProperties().getString(ResultParameterNames.Json) +
               "> LanguageUnderstandingJson <" + getProperties().getString(ResultParameterNames.LanguageUnderstandingJson) +
                ">.";
    }
    
    // Hold the reference.
    private IntentRecognitionEventArgs eventArgImpl;
}
