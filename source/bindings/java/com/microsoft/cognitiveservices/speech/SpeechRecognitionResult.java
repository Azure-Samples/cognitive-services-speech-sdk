package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.internal.RecognitionResult;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyCollection;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.ResultProperties;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

import java.math.BigInteger;

/**
  * Defines result of speech recognition.
  */
public class SpeechRecognitionResult {

    private String resultId;
    private ResultReason reason;
    private String text;
    private ResultProperties properties;
    private long duration;
    private long offset;
    private RecognitionResult _resultImpl;

    protected SpeechRecognitionResult(RecognitionResult result) {
        Contracts.throwIfNull(result, "result");

        this._resultImpl = result;
        this.resultId = result.getResultId();
        Contracts.throwIfNull(resultId, "resultId");

        BigInteger tenThousand = BigInteger.valueOf(10000);
        this.duration = result.Duration().divide(tenThousand).longValue();
        this.offset = result.Offset().divide(tenThousand).longValue();

        this.text = result.getText();
        this.reason = ResultReason.values()[result.getReason().swigValue()];

        this.properties = new ResultProperties(result.getProperties());
    }

    /**
      * Explicitly frees any external resource attached to the object
      */
    public void close() {
        if (this._resultImpl != null) {
            this._resultImpl.delete();
        }
        this._resultImpl = null;

        if (this.properties != null) {
            this.properties.close();
        }
        this.properties = null;
    }

    /**
      * Specifies the result identifier.
      * @return Specifies the result identifier.
      */
    public String getResultId() {
        return this.resultId;
    }
    
    /**
      * Specifies reason the result was created.
      * @return Specifies reason of the result.
      */
    public ResultReason getReason() {
        return this.reason;
    }

    /**
      * Presents the recognized text in the result.
      * @return Presents the recognized text in the result.
      */
    public String getText() {
        return this.text;
    }

    /**
      * Duration of recognized speech in milliseconds.
      * @return Duration of recognized speech in milliseconds.
      */
    public long getDuration() {
        return this.duration;
    }

    /**
      * Offset of recognized speech in milliseconds.
      * @return Offset of recognized speech in milliseconds.
      */
    public long getOffset() {
        return this.offset;
    }

    /**
    * A string containing Json serialized recognition result as it was received from the service.
    * @return Json serialized representation of the result.
    */
    public String getJson() {
      return this.properties.getProperty(SpeechPropertyId.SpeechServiceResponse_Json);
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public PropertyCollection getProperties() {
        return this.properties;
    }

    /**
      * Returns a String that represents the speech recognition result.
      * @return A String that represents the speech recognition result.
      */
    @Override
    public String toString() {
        return "ResultId:" + this.resultId +
               " Status:" + this.reason +
               " Recognized text:<" + this.text +
               ">.";
    }

    /**
      * Returns the recognition result implementation.
      * @return The implementation of the result.
      */
    public com.microsoft.cognitiveservices.speech.internal.RecognitionResult getResultImpl() {
        return this._resultImpl;
    }
}
