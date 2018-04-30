package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ResultParameterNames;
import com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyValueCollection;

/**
  * Defines content of an intent intermediate/final result events.
  */
public final class IntentRecognitionResultEventArgs // : System.EventArgs
{
    IntentRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs e)
    {
        this.eventArgImpl = e;
        this._Result = new IntentRecognitionResult(e.getResult());
        this._SessionId = e.getSessionId();
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
        return eventArgImpl.getResult().getProperties();
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
               "> Recognized text:<" + _Result.getRecognizedText() +
               "> Recognized json:<" + getProperties().getString(ResultParameterNames.Json) +
               "> LanguageUnderstandingJson <" + getProperties().getString(ResultParameterNames.LanguageUnderstandingJson) +
                ">.";
    }
    
    // Hold the reference.
    private IntentRecognitionEventArgs eventArgImpl;
}
