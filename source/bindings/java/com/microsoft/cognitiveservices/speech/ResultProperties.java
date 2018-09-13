package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.internal.RecognitionResult;
import com.microsoft.cognitiveservices.speech.internal.ResultPropertyCollection;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

/**
 * Result property collection. TODO: Merge with RecognizerProperties
 */
public final class ResultProperties implements PropertyCollection, Closeable {
    
    public ResultProperties(ResultPropertyCollection collection) {
        Contracts.throwIfNull(collection, "collection");
        this.collection = collection;
    }

    /**
      * Returns the property value.
      * If the name is not available, it returns an empty String.
      *
      * @param name The property name.
      * @return value of the property.
      */
    public String getProperty(String name) {
        return getProperty(name, "");
    }

    /**
      * Returns the property value.
      * If the name is not available, it returns a default value.
      *
      * @param name The property name.
      * @param defaultValue The default value which is returned if the property is not available in the collection.
      * @return value of the property.
      */
    public String getProperty(String name, String defaultValue) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        
        String ret = collection.GetProperty(name, defaultValue);
        return ret;
    }

    /**
      * Gets the property's value by its id
      *
      * @param id The speech property id
      * @return The value of the property.
      */
    public String getProperty(SpeechPropertyId id) {
        return collection.GetProperty(id.getValue());
    }

    /**
      * Sets the propery value by name
      *
      * @param name The property name.
      * @param value The value of the property.
      */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNull(value, "value");
        
        collection.SetProperty(name, value);
    }

    /**
      * Sets the property's value by id
      *
      * @param id The speech property id
      * @param value The value of the parameter.
      */
    public void setProperty(SpeechPropertyId id, String value) {
        Contracts.throwIfNull(value, "value");
        
        collection.SetProperty(id.getValue(), value);
    }

    /**
      * Dispose of the associated parameter value collection.
      *
      */
    public void close() {

        if (collection != null) {
            collection.delete();
        }

        collection = null;
    }

    private com.microsoft.cognitiveservices.speech.internal.ResultPropertyCollection collection;
}
