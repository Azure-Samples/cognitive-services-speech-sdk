package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ResultParameterNames;
import com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Defines content of an intent recognizing/recognized events.
  */
public final class IntentRecognitionResultEventArgs // : System.EventArgs
{
    IntentRecognitionResultEventArgs(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionEventArgs eventArg)
    {
        Contracts.throwIfNull(eventArg, "eventArg");

        this.eventArgImpl = eventArg;
        this._Result = new IntentRecognitionResult(eventArg.GetResult());
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
    }
    private IntentRecognitionResult  _Result;

    /**
      * A String represents the session identifier.
      * @return A String represents the session identifier.
      */
    public final String getSessionId()
    {
        return _SessionId;
    }
   
    private String _SessionId;
    
    public ResultPropertyCollection getProperties()
    {
        ResultPropertyCollection r = eventArgImpl.GetResult().getProperties();
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
               " Reason:" + _Result.getReason() +
               " IntentId:<" + _Result.getIntentId() +
               "> Recognized text:<" + _Result.getText() +
               "> Recognized json:<" + getProperties().GetProperty(PropertyId.SpeechServiceResponse_Json.getValue()) +
               "> LanguageUnderstandingJson <" + getProperties().GetProperty(PropertyId.SpeechServiceResponse_JsonResult.getValue()) +
                ">.";
    }
    
    // Hold the reference.
    private IntentRecognitionEventArgs eventArgImpl;
}
