//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;

import java.io.Closeable;
import java.io.IOException;

/*! \cond INTERNAL */

/**
 * Internal IntRef class
 */

public class IntRef {

    public IntRef(long val) {
        value = val;
    }

    /**
     * Sets the long value
     * @param newValue Long value to set.
     * @return
     */
    public void setValue(long newValue) {
        this.value = newValue;
    }

    /**
     * Gets the long value
     * @return long value
     */
    public long getValue() {
        return this.value;
    }

    private long value;
}

/*! \endcond */