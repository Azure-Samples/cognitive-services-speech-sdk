//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;

/*! \cond INTERNAL */

/**
 * Internal StringRef class
 */

public class StringRef {

    public StringRef(String val) {
        value = val;
    }
    /**
     * Sets the String value
     * @param newValue String value to set.
     * @return
     */
    public void setValue(String newValue) {
        this.value = newValue ;
    }

    /**
     * Gets the String value
     * @return String value
     */
    public String getValue() {
        return this.value ;
    }

    private String value;
}

/*! \endcond */