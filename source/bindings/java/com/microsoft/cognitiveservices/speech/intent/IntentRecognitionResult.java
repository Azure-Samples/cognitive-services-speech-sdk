package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ResultParameterNames;

/**
  * Defines result of intent recognition.
  */
public final class IntentRecognitionResult extends com.microsoft.cognitiveservices.speech.SpeechRecognitionResult {
    
    IntentRecognitionResult(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult result) {
        super(result);
        this._intentId = result.getIntentId();
    }

    /**
      * A String that represents the intent identifier being recognized.
      * @return A String that represents the intent identifier being recognized.
      */
    public String getIntentId() {
        return _intentId;
    } // { get; }
    private String _intentId;

    /**
     * A String that represents the intent including properties being recognized.
     * @return A String that represents the intent including properties being recognized.
     */
    public String getLanguageUnderstanding() {
        return getProperties().getString(ResultParameterNames.LanguageUnderstandingJson);
    } // { get; }

   /**
    * A String that represents the intent as json.
    * @return A String that represents the intent as json.
    */
    public String getJson() {
      return getProperties().getString(ResultParameterNames.Json);
    } // { get; }

    /**
     * A String that represents error result in case the call failed.
     * @return A String that represents error result in case the call failed.
     */
     public String getErrorDetails() {
       return getProperties().getString(ResultParameterNames.ErrorDetails);
     } // { get; }
    
    /**
      * Returns a String that represents the intent recognition result.
      * @return A String that represents the intent recognition result.
      */
    @Override
    public String toString() {
        return "ResultId:" + getResultId() +
               " Status:" + getReason() +
               " IntentId:<" + _intentId +
               "> Recognized text:<" + getText() +
               "> Recognized json:<" + getProperties().getString(ResultParameterNames.Json) +
               "> LanguageUnderstandingJson <" + getProperties().getString(ResultParameterNames.LanguageUnderstandingJson) +
               ">.";
    }
}
