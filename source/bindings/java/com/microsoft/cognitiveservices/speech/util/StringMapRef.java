//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;
import java.util.HashMap;
import java.util.Map;

/*! \cond INTERNAL */

/**
 * Internal StringMapRef class
 */

public class StringMapRef {

    public StringMapRef() {
        stringMap = new HashMap<String, String>();
    }
    /**
     * Sets the String value
     * @param newValue String value to set.
     * @return
     */
    public void setValue(String key, String value) {
        this.stringMap.put(key, value);
    }

    /**
     * Gets the String value
     * @return String value
     */
    public Map<String, String> getValue() {
        return this.stringMap;
    }

    private Map<String, String> stringMap;
}

/*! \endcond */