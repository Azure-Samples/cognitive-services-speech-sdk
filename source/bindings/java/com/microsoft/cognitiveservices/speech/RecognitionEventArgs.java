//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.math.*;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines payload for recognition events like Speech Start/End Detected
 */
public class RecognitionEventArgs extends SessionEventArgs
{

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a RecognitionEventArgs object.
     * @param arg internal recognition event args object.
     */
    public RecognitionEventArgs(com.microsoft.cognitiveservices.speech.internal.RecognitionEventArgs arg) {
        super(arg);

        Contracts.throwIfNull(arg, "arg");
        this.offset = arg.getOffset();
    }

    /*! \endcond */

    /**
     * Represents the message offset in 100nsec increments.
     */
    public final BigInteger offset;

    /**
     * Returns a String that represents the recognition event payload.
     * @return A String that represents the recognition event payload.
     */
    @Override
    public String toString() {
        return "SessionId: " + this.getSessionId() + " Offset: " + offset.toString() + ".";
    }
}
