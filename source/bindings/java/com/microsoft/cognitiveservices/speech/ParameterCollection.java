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
      * If the name is not available, the specified defaultValue is returned.
      *
      * @param name The parameter name.
      * @param defaultValue The default value which is returned if the parameter is not available in the collection.
      * @return value of the parameter.
      */
    public String getString(String name, String defaultValue) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        if(factoryParameters != null) {
            String ret = factoryParameters.GetProperty(name, defaultValue);
            return ret;
        }
        else {
            String ret = recognizerParameters.GetProperty(name, defaultValue);
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
            factoryParameters.SetProperty(name, value);
        }
        else {
            recognizerParameters.SetProperty(name, value);
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

        if (recognizerParameters != null) {
            recognizerParameters.delete();
        }
        recognizerParameters = null;

        if (factoryParameters != null) {
            factoryParameters.delete();
        }
        factoryParameters = null;

        disposed = true;
    }

    private com.microsoft.cognitiveservices.speech.internal.RecognizerPropertyCollection recognizerParameters;
    private com.microsoft.cognitiveservices.speech.internal.FactoryPropertyCollection factoryParameters;
    private boolean disposed = false;
}
