package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;


//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Represents collection of parameters and their values.
 */
public final class ParameterCollection<OwnerType> implements Closeable
{
    public ParameterCollection(OwnerType owner) throws UnsupportedOperationException
    {
        if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.recognition.RecognizerFactory.class))
        {
            isFactoryParameter = true;
            speechParameters = null;
        }
        else if (owner.getClass().equals(com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognizer.class))
        {
            isFactoryParameter = false;
            com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognizer speechRecognizer = (com.microsoft.cognitiveservices.speech.recognition.speech.SpeechRecognizer)owner;
            speechParameters = speechRecognizer.getRecoImpl().getParameters();
        }
        else
        {
            throw new UnsupportedOperationException("ParameterCollection: Unsupported type: " + owner.getClass());
        }
    }

    /**
      * Checks whether the parameter specified by name has a String value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isString(String name)
    {
        return isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsString(name) : speechParameters.containsString(name);
    }

    /**
      * Checks whether the parameter specified by name has an Int value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isInt(String name)
    {
        return isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsNumber(name) : speechParameters.containsNumber(name);
    }

    /**
      * Checks whether the parameter specified by name has a Boolean value.
      *
      * @param name The parameter name.
      * @return true if the parameter has a value, and false otherwise.
      */
    public boolean isBool(String name)
    {
        return isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsBool(name) : speechParameters.containsBool(name);
    }

    /**
      * Returns the parameter value in type String. The parameter must have the same type as String.
      * Currently only String, int and bool are allowed.
      * If the name is not available, it returns an empty String if the parameter is a String, 0 if the parameter is int, 
      * and false if the parameter is bool.
      *
      * @param name The parameter name.
      * @return value of the parameter.
      */
    public String getString(String name)
    {
        return getString(name, "");
    }
    public int getInt(String name)
    {
        return getInt(name, 0);
    }
    
    public boolean getBool(String name)
    {
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
    public String getString(String name, String defaultValue)
    {
        String ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getString(name, defaultValue) : speechParameters.getString(name, defaultValue);
        return ret;
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
    public int getInt(String name, int defaultValue)
    {
        int ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getNumber(name, defaultValue) : speechParameters.getNumber(name, defaultValue);
        return ret;
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
    public boolean getBool(String name, boolean defaultValue)
    {
        boolean ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getBool(name, defaultValue) : speechParameters.getBool(name, defaultValue);
        return ret;
    }

    /**
      * Sets the String value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, String value)
    {
        if (isFactoryParameter)
        {
            com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.setString(name, value);
        }
        else
        {
            speechParameters.setString(name, value);
        }
    }

    /**
      * Sets the integer value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, int value)
    {
        if (isFactoryParameter)
        {
            com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.setNumber(name, value);
        }
        else
        {
            speechParameters.setNumber(name, value);
        }
    }

    /**
      * Sets the Boolean value of the parameter specified by name.
      *
      * @param name The parameter name.
      * @param value The value of the parameter.
      */
    public void set(String name, boolean value)
    {
        if (isFactoryParameter)
        {
            com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.setBool(name, value);
        }
        else
        {
            speechParameters.setBool(name, value);
        }
    }

    /**
      * Dispose of the associated parameter value collection.
      *
      */
    public void close()
    {
        if (disposed)
        {
            return;
        }

        if(speechParameters != null)
            speechParameters.delete();
        disposed = true;
    }

    private com.microsoft.cognitiveservices.speech.internal.RecognizerParameterValueCollection speechParameters;
    private boolean disposed = false;
    private boolean isFactoryParameter = false;
}
