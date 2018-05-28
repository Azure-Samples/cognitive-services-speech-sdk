package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.internal.RecognitionResult;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyValueCollection;

/**
  * Defines result of speech recognition.
  */
public class SpeechRecognitionResult {

    private String resultId;
    private RecognitionStatus reason;
    private String text;
    private RecognitionResultCollection properties;

    protected SpeechRecognitionResult(RecognitionResult result) {
        resultId = result.getResultId();
        text = result.getText();
        reason = RecognitionStatus.values()[result.getReason().swigValue()];
        properties = new RecognitionResultCollection(result.getProperties());
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
    public String getText() {
        return text;
    }

    /**
    * In case of an unsuccessful recognition, provides a brief description of an occurred error.
    * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
    * @return a brief description of an error.
    */
    public String getErrorDetails() {
        return properties.getString(ResultParameterNames.ErrorDetails);
    }

    /**
    * A string containing Json serialized recognition result as it was received from the service.
    * @return Json serialized representation of the result.
    */
    public String getJson() {
      return properties.getString(ResultParameterNames.Json);
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public RecognitionResultCollection getProperties() {
        return properties;
    }

    /**
      * Returns a String that represents the speech recognition result.
      * @return A String that represents the speech recognition result.
      */
    @Override
    public String toString() {
        return "ResultId:" + resultId +
               " Status:" + reason +
               " Recognized text:<" + text +
               ">.";
    }
    
    public class RecognitionResultCollection
    {
        ResultPropertyValueCollection _collection;
        
        RecognitionResultCollection(ResultPropertyValueCollection collection)
        {
            _collection = collection;
        }
        
        /**
         * Checks whether the parameter specified by name has a String value.
         *
         * @param name The parameter name.
         * @return true if the parameter has a value, and false otherwise.
         */
       public boolean isString(String name) {
           
           return _collection.containsString(name);
       }

       /**
         * Checks whether the parameter specified by name has an Int value.
         *
         * @param name The parameter name.
         * @return true if the parameter has a value, and false otherwise.
         */
       public boolean isInt(String name) {
           
           return _collection.containsNumber(name);
       }

       /**
         * Checks whether the parameter specified by name has a Boolean value.
         *
         * @param name The parameter name.
         * @return true if the parameter has a value, and false otherwise.
         */
       public boolean isBool(String name) {
           
           return _collection.containsBool(name);
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
        * Returns the parameter value in type int. The parameter must have the same type as int.
        * If the name is not available, it returns 0.
        *
        * @param name The parameter name.
        * @return value of the parameter.
        */
       public int getInt(String name) {
           return getInt(name, 0);
       }
       
       /**
        * Returns the parameter value in type boolean. The parameter must have the same type as boolean.
        * If the name is not available, it returns false.
        *
        * @param name The parameter name.
        * @return value of the parameter.
        */
       public boolean getBool(String name) {
           return getBool(name, false);
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
           return _collection.getString(name, defaultValue);
       }

       /**
         * Returns the parameter value in type int. The parameter must have the same type as int.
         * Currently only String, int and bool are allowed.
         * If the name is not available, the specified defaultValue is returned.
         *
         * @param name The parameter name.
         * @param defaultValue The default value which is returned if the parameter is not available in the collection.
         * @return value of the parameter.
         */
       public int getInt(String name, int defaultValue) {
           return _collection.getNumber(name, defaultValue);
       }

       /**
         * Returns the parameter value in type boolean. The parameter must have the same type as boolean.
         * Currently only String, int and bool are allowed.
         * If the name is not available, the specified defaultValue is returned.
         *
         * @param name The parameter name.
         * @param defaultValue The default value which is returned if the parameter is not available in the collection.
         * @return value of the parameter.
         */
       public boolean getBool(String name, boolean defaultValue) {
           return _collection.getBool(name, defaultValue);
       }
    }
}
