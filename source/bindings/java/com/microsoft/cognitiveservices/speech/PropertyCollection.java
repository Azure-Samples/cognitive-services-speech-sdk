//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.PropertyId;

/**
 * Represents collection of properties and their values.
 */
public class PropertyCollection implements Closeable {

    /*! \cond PROTECTED */

    public PropertyCollection(IntRef propHandle) {
        if(propHandle != null) {
            propertyHandle = new SafeHandle(propHandle.getValue(), SafeHandleType.PropertyCollection);
        }
    }

    /*! \endcond */

    /**
     * Returns the property value.
     * If the name is not available, it returns an empty String.
     *
     * @param name The property name.
     * @return value of the property.
     */
    public String getProperty(String name) {   
        return getPropertyString(propertyHandle, -1, name, "");
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
        Contracts.throwIfNull(propertyHandle, "collection");
        Contracts.throwIfNullOrWhitespace(name, "name");
        return getPropertyString(propertyHandle, -1, name, defaultValue);
    }

    /**
     * Gets the property's value by its id
     *
     * @param id The speech property id
     * @return The value of the property.
     */
    public String getProperty(PropertyId id) {
        Contracts.throwIfNull(propertyHandle, "collection");
        return getPropertyString(propertyHandle, id.getValue(), null, "");
    }

    /**
     * Sets the property value by name
     *
     * @param name The property name.
     * @param value The value of the property.
     */
    public void setProperty(String name, String value) {
        Contracts.throwIfNull(propertyHandle, "collection");
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNull(value, "value");
        Contracts.throwIfFail(setPropertyString(propertyHandle, -1, name, value));
    }

    /**
     * Sets the property's value by id
     *
     * @param id The property id
     * @param value The value of the parameter.
     */
    public void setProperty(PropertyId id, String value) {
        Contracts.throwIfNull(propertyHandle, "collection");
        Contracts.throwIfNull(value, "value");
        Contracts.throwIfFail(setPropertyString(propertyHandle, id.getValue(), null, value));
    }

    /**
     * Dispose of the associated parameter value collection.
     */
    @Override
    public void close() {

        if (propertyHandle != null) {
            propertyHandle.close();            
            propertyHandle = null;
        }
    }

    private final native long setPropertyString(SafeHandle propertyHandle, int id, String name, String defaultValue);
    private final native String getPropertyString(SafeHandle propertyHandle, int id, String name, String defaultValue);

    private SafeHandle propertyHandle = null;
}
