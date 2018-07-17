package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;


/**
 * Represents collection of parameters and their values.
 */
public final class ParameterCollection<OwnerType> implements Closeable {
    
    public ParameterCollection(OwnerType owner) {
        Contracts.throwIfNull(owner, "owner");
        
        if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.SpeechFactory.class)) {
            com.microsoft.cognitiveservices.speech.SpeechFactory speechFactory = (com.microsoft.cognitiveservices.speech.SpeechFactory)owner;
            factoryParameters = speechFactory.getFactoryImpl().getParameters();
        }
        else if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.SpeechRecognizer.class)) {
            com.microsoft.cognitiveservices.speech.SpeechRecognizer speechRecognizer = (com.microsoft.cognitiveservices.speech.SpeechRecognizer)owner;
            recognizerParameters = speechRecognizer.getRecoImpl().getParameters();
        }
        else if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.intent.IntentRecognizer.class)) {
            com.microsoft.cognitiveservices.speech.intent.IntentRecognizer intentRecognizer = (com.microsoft.cognitiveservices.speech.intent.IntentRecognizer)owner;
            recognizerParameters = intentRecognizer.getRecoImpl().getParameters();
        }
        else if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer.class)) {
            com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer translateRecognizer = (com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer)owner;
            recognizerParameters = translateRecognizer.getRecoImpl().getParameters();
        }
        else {
            throw new UnsupportedOperationException("ParameterCollection: Unsupported type: " + owner.getClass());
        }
    }

    /**
      * Checks whether the parameter specified by name has a String value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isString(String name) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            return factoryParameters.ContainsString(name);
        }
        else {
            return recognizerParameters.ContainsString(name);
        }
    }

    /**
      * Checks whether the parameter specified by name has an Int value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isInt(String name) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            return factoryParameters.ContainsNumber(name);
        }
        else {
            return recognizerParameters.ContainsNumber(name);
        }
    }

    /**
      * Checks whether the parameter specified by name has a Boolean value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isBool(String name) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            return factoryParameters.ContainsBool(name);
        }
        else {
            return recognizerParameters.ContainsBool(name);
        }
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
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            String ret = factoryParameters.GetString(name, defaultValue);
            return ret;
        }
        else {
            String ret = recognizerParameters.GetString(name, defaultValue);
            return ret;
        }
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
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            int ret = factoryParameters.GetNumber(name, defaultValue);
            return ret;
        }
        else {
            int ret = recognizerParameters.GetNumber(name, defaultValue);
            return ret;
        }
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
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            boolean ret = factoryParameters.GetBool(name, defaultValue);
            return ret;
        }
        else {
            boolean ret = recognizerParameters.GetBool(name, defaultValue);
            return ret;
        }
    }

    /**
      * Sets the String value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNull(value, "value");
        
        if(factoryParameters != null) {
            factoryParameters.SetString(name, value);
        }
        else {
            recognizerParameters.SetString(name, value);
        }
    }

    /**
      * Sets the integer value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, int value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            factoryParameters.SetNumber(name, value);
        }
        else {
            recognizerParameters.SetNumber(name, value);
        }
    }

    /**
      * Sets the Boolean value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, boolean value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            factoryParameters.SetBool(name, value);
        }
        else {
            recognizerParameters.SetBool(name, value);
        }
    }

    /**
      * Dispose of the associated parameter value collection.
      *
      */
    public void close() {
        
        if (disposed) {
            return;
        }

        if(recognizerParameters != null)
            recognizerParameters.delete();
        if(factoryParameters != null)
            factoryParameters.delete();
        disposed = true;
    }

    private com.microsoft.cognitiveservices.speech.internal.RecognizerParameterValueCollection recognizerParameters;
    private com.microsoft.cognitiveservices.speech.internal.FactoryParameterCollection factoryParameters;
    private boolean disposed = false;
}
