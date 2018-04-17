package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;


//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/// <summary>
/// Represents collection of parameters and their values.
/// </summary>
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

    /// <summary>
    /// Checks whether the parameter specified by name has a <typeparamref name="T"/> value.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <returns>true if the parameter has a <typeparamref name="T"/> value, and false otherwise.</returns>
    public boolean isString(String name)
    {
        return false; //isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsString(name) : speechParameters.containsString(name);
    }
    public boolean isInt(String name)
    {
        return false; //isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsNumber(name) : speechParameters.containsNumber(name);
    }
    public boolean isBool(String name)
    {
        return false; //isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.containsBool(name) : speechParameters.containsBool(name);
    }

    /// <summary>
    /// Returns the parameter value in type <typeparamref name="T"/>. The parameter must have the same type as <typeparamref name="T"/>.
    /// Currently only String, int and bool are allowed.
    /// If the name is not available, it returns an empty String if the parameter is a String, 0 if the parameter is int, 
    /// and false if the parameter is bool.
    /// </summary>
    /// <typeparam name="T">The type of parameter. only String, int and bool are supported.</typeparam>
    /// <param name="name">The parameter name.</param>
    /// <returns>value of the parameter.</returns>
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
    
    /// <summary>
    /// Returns the parameter value in type <typeparamref name="T"/>. The parameter must have the same type as <typeparamref name="T"/>.
    /// Currently only String, int and bool are allowed.
    /// If the name is not available, the specified defaultValue is returned.
    /// </summary>
    /// <typeparam name="T">The type of parameter. only String, int and bool are supported.</typeparam>
    /// <param name="name">The parameter name.</param>
    /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
    /// <returns>value of the parameter.</returns>
    public String getString(String name, String defaultValue)
    {
        String ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getString(name, defaultValue) : speechParameters.getString(name, defaultValue);
        return ret;
    }
    public int getInt(String name, int defaultValue)
    {
        int ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getNumber(name, defaultValue) : speechParameters.getNumber(name, defaultValue);
        return ret;
    }
    public boolean getBool(String name, boolean defaultValue)
    {
        boolean ret = isFactoryParameter ? com.microsoft.cognitiveservices.speech.internal.DefaultRecognizerFactory.Parameters.getBool(name, defaultValue) : speechParameters.getBool(name, defaultValue);
        return ret;
    }

    /// <summary>
    /// Sets the String value of the parameter specified by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <param name="value">The value of the parameter.</param>
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

    /// <summary>
    /// Sets the integer value of the parameter specified by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <param name="value">The value of the parameter.</param>
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

    /// <summary>
    /// Sets the boolean value of the parameter specified by name.
    /// </summary>
    /// <param name="name">The parameter name.</param>
    /// <param name="value">The value of the parameter.</param>
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

    /// <summary>
    /// Dispose of the associated parameter value collection.
    /// </summary>
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
