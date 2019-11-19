//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import java.util.HashMap;
import java.util.Map;

class RemoteConversationTranscriptionPropertyCollectionImpl extends PropertyCollection {
    public RemoteConversationTranscriptionPropertyCollectionImpl() {
        super(null);
    }

    /**
     * Returns the property value.
     * If the name is not available, it returns an empty String.
     *
     * @param name The property name.
     * @return value of the property.
     */
    @Override
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
    @Override
    public String getProperty(String name, String defaultValue) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        if(localCollectionString.containsKey(name)) {
            return localCollectionString.get(name);
        }
        else {
            return defaultValue;
        }
    }

    /**
     * Gets the property's value by its id
     *
     * @param id The speech property id
     * @return The value of the property.
     */
    @Override
    public String getProperty(PropertyId id) {
        return localCollectionPropertyId.get(id);
    }

    /**
     * Sets the property value by name
     *
     * @param name The property name.
     * @param value The value of the property.
     */
    @Override
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNull(value, "value");
        localCollectionString.put(name,value);
    }

    /**
     * Sets the property's value by id
     *
     * @param id The property id
     * @param value The value of the parameter.
     */
    @Override
    public void setProperty(PropertyId id, String value) {
        Contracts.throwIfNull(value, "value");
        localCollectionPropertyId.put(id, value);
    }

    /**
     * Dispose of the associated parameter value collection.
     */
    @Override
    public void close() {
        super.close();
    }
    private Map<String, String> localCollectionString = new HashMap<String, String>();
    private Map<PropertyId, String> localCollectionPropertyId = new HashMap<PropertyId, String>();
}
