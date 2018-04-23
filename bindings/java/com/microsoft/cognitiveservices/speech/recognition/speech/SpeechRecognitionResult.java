package com.microsoft.cognitiveservices.speech.recognition.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.recognition.RecognitionStatus;

/**
  * Defines result of speech recognition.
  */
public class SpeechRecognitionResult 
{
    protected SpeechRecognitionResult(com.microsoft.cognitiveservices.speech.internal.RecognitionResult result)
    {
        this._ResultId = result.getResultId();
        this._RecognizedText = result.getText();
        this._Reason = RecognitionStatus.values()[result.getReason().swigValue()];
    }

    /**
      * Specifies the result identifier.
      * @return Specifies the result identifier.
      */
    public String getResultId()
    {
        return _ResultId;
    }// { get; }
    private String _ResultId;

    /**
      * Specifies status of the result.
      * @return Specifies status of the result.
      */
    public RecognitionStatus getReason() // { get; }
    {
        return _Reason;
    }
    private RecognitionStatus _Reason;

    /**
      * Presents the recognized text in the result.
      * @return Presents the recognized text in the result.
      */
    public String getRecognizedText() {
        return _RecognizedText;
    } // { get; }
    private String _RecognizedText;

    /**
      * Returns a String that represents the speech recognition result.
      * @return A String that represents the speech recognition result.
      */
    @Override
    public String toString()
    {
        return "ResultId:" + _ResultId + " Status:" + _Reason + " Recognized text:<" + _RecognizedText + ">.";
    }
}
