package com.microsoft.cognitiveservices.speech.recognition.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines result of intent recognition.
  */
public class IntentRecognitionResult extends com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognitionResult
{
    IntentRecognitionResult(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult result)
    {
        super(result);
        this._intentId = result.getIntentId();
    }

    /**
      * A String that represents the intent identifier being recognized.
      * @return A String that represents the intent identifier being recognized.
      */
    public String getIntentId() // { get; }
    {
        return _intentId;
    }
    private String _intentId;

    /**
      * Returns a String that represents the intent recognition result.
      * @return A String that represents the intent recognition result.
      */
    @Override
    public String toString()
    {
        return "ResultId:" + getResultId() + " Status:" + getReason() + " IntentId:<" + _intentId + "> Recognized text:<" + getRecognizedText() + ">.";
    }
}
