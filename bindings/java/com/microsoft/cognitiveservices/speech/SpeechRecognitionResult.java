package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.internal.RecognitionResult;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyValueCollection;
import com.microsoft.cognitiveservices.speech.internal.ResultProperty;

/**
  * Defines result of speech recognition.
  */
public class SpeechRecognitionResult {

    private String resultId;
    private RecognitionStatus reason;
    private String recognizedText;
    private ResultPropertyValueCollection properties;

    protected SpeechRecognitionResult(RecognitionResult result) {
        resultId = result.getResultId();
        recognizedText = result.getText();
        reason = RecognitionStatus.values()[result.getReason().swigValue()];
        properties = result.getProperties();
    }

    /**
      * Specifies the result identifier.
      * @return Specifies the result identifier.
      */
    public String getResultId() {
        return resultId;
    }
    
    /**
      * Specifies status of the result.
      * @return Specifies status of the result.
      */
    public RecognitionStatus getReason() {
        return reason;
    }
    

    /**
      * Presents the recognized text in the result.
      * @return Presents the recognized text in the result.
      */
    public String getRecognizedText() {
        return recognizedText;
    }

    /**
    * In case of an unsuccessful recognition, provides a brief description of an occurred error.
    * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
    * @return a brief description of an error.
    */
    public String getErrorDetails() {
        return properties.get(ResultProperty.ErrorDetails).getString();
    }

    /**
    * A string containing Json serialized recognition result as it was received from the service.
    * @return Json serialized representation of the result.
    */
    public String getJson() {
      return properties.get(ResultProperty.Json).getString();
    }
    
    /**
      * Returns a String that represents the speech recognition result.
      * @return A String that represents the speech recognition result.
      */
    @Override
    public String toString() {
        return "ResultId:" + resultId + " Status:" + reason + " Recognized text:<" + recognizedText + ">.";
    }
}
