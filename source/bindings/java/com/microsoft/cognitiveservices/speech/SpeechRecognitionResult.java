package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.internal.RecognitionResult;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyCollection;
import com.microsoft.cognitiveservices.speech.util.Contracts;

import java.math.BigInteger;

/**
  * Defines result of speech recognition.
  */
public class SpeechRecognitionResult {

    private String resultId;
    private RecognitionStatus reason;
    private String text;
    private RecognitionResultCollection properties;
    private long duration;
    private long offset;
    private String errorDetails;
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
        this.reason = RecognitionStatus.values()[result.getReason().swigValue()];
        this.errorDetails = result.getErrorDetails();

        this.properties = new RecognitionResultCollection(result.getProperties());
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
      * Specifies status of the result.
      * @return Specifies status of the result.
      */
    public RecognitionStatus getReason() {
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
    * In case of an unsuccessful recognition, provides a brief description of an occurred error.
    * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
    * @return a brief description of an error.
    */
    public String getErrorDetails() {
        return this.errorDetails;
    }

    /**
    * A string containing Json serialized recognition result as it was received from the service.
    * @return Json serialized representation of the result.
    */
    public String getJson() {
      return this.properties.getString(ResultParameterNames.Json);
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public RecognitionResultCollection getProperties() {
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
    
    public class RecognitionResultCollection
    {
        ResultPropertyCollection _collection;
        
        RecognitionResultCollection(ResultPropertyCollection collection)
        {
            Contracts.throwIfNull(collection, "collection");

            this._collection = collection;
        }
        
        /**
          * Explicitly frees any external resource attached to the object
          */
        public void close() {
            if (this._collection != null) {
                this._collection.delete();
            }
            this._collection = null;
        }

       /**
         * Returns the parameter value in type String. The parameter must have the same type as String.
         * If the name is not available, it returns an empty String.
         *
         * @param name The parameter name.
         * @return value of the parameter.
         */
       public String getString(String name) {
           return getString(name, "");
       }
       
       
       /**
         * Returns the parameter value in type String. The parameter must have the same type as String.
         * Currently only String, int and bool are allowed.
         * If the name is not available, the specified defaultValue is returned.
         *
         * @param name The parameter name.
         * @param defaultValue The default value which is returned if the parameter is not available in the collection.
         * @return value of the parameter.
         */
       public String getString(String name, String defaultValue) {
           Contracts.throwIfNull(name, "name");

           return _collection.GetProperty(name, defaultValue);
       }
    }
}
