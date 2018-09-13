package com.microsoft.cognitiveservices.speech.intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.ResultParameterNames;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;

/**
  * Defines result of intent recognition.
  */
public final class IntentRecognitionResult extends com.microsoft.cognitiveservices.speech.SpeechRecognitionResult {
    private com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult _resultImpl;
    
    IntentRecognitionResult(com.microsoft.cognitiveservices.speech.internal.IntentRecognitionResult result) {
        super(result);
        Contracts.throwIfNull(result, "result");

        this._resultImpl = result;
        this._intentId = result.getIntentId();
        Contracts.throwIfNull(this._intentId, "IntentId");
    }

    /**
      * Explicitly frees any external resource attached to the object
      */
    @Override
    public void close() {
        if (this._resultImpl != null) {
            this._resultImpl.delete();
        }
        this._resultImpl = null;

        super.close();
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
        return getProperties().getProperty(SpeechPropertyId.SpeechServiceResponse_JsonResult);
    } // { get; }

   /**
    * A String that represents the intent as json.
    * @return A String that represents the intent as json.
    */
    public String getJson() {
      return getProperties().getProperty(SpeechPropertyId.SpeechServiceResponse_Json);
    } // { get; }

    /**
     * A String that represents error result in case the call failed.
     * @return A String that represents error result in case the call failed.
     */
     public String getErrorDetails() {
       return getProperties().getProperty(SpeechPropertyId.SpeechServiceResponse_JsonErrorDetails);
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
               "> Recognized json:<" + getProperties().getProperty(SpeechPropertyId.SpeechServiceResponse_Json) +
               "> LanguageUnderstandingJson <" + getProperties().getProperty(SpeechPropertyId.SpeechServiceResponse_JsonResult) +
               ">.";
    }
}
