package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechPropertyId;

/**
 * Represents collection of properties.
 */
public interface PropertyCollection {
    /**
      * Returns the property value.
      * If the name is not available, it returns an empty String.
      *
      * @param name The property name.
      * @return value of the property.
      */
    public String getProperty(String name);

    /**
      * Returns the property value.
      * If the name is not available, it returns a default value.
      *
      * @param name The property name.
      * @param defaultValue The default value which is returned if the property is not available in the collection.
      * @return value of the property.
      */
    public String getProperty(String name, String defaultValue);

    /**
      * Gets the property's value by its id
      *
      * @param id The speech property id
      * @return The value of the property.
      */
    public String getProperty(SpeechPropertyId id);

    /**
      * Sets the propery value by name
      *
      * @param name The property name.
      * @param value The value of the property.
      */
    public void setProperty(String name, String value);

    /**
      * Sets the property's value by id
      *
      * @param id The speech property id
      * @param value The value of the parameter.
      */
    public void setProperty(SpeechPropertyId id, String value);
}
