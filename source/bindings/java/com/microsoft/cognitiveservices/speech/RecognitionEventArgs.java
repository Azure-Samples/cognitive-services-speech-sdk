//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.math.*;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Defines payload for recognition events like Speech Start/End Detected
 */
public class RecognitionEventArgs extends SessionEventArgs {

    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a RecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    public RecognitionEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);
    }

    /**
     * Constructs an instance of a RecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    RecognitionEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * Represents the message offset in 100nsec increments.
     */
    public BigInteger offset;

    /**
     * Returns a String that represents the recognition event payload.
     * @return A String that represents the recognition event payload.
     */
    @Override
    public String toString() {
        return "SessionId: " + this.getSessionId() + " Offset: " + offset.toString() + ".";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef errorHandle = new IntRef(0);
        offset = getOffset(eventHandle, errorHandle);
        Contracts.throwIfFail(errorHandle.getValue());
        if (disposeNativeResources == true)
        {
            super.close();
        }
    }

    /*! \cond PROTECTED */
    protected final native long getRecognitionResult(SafeHandle eventHandle, IntRef resultHandle);
    /*! \endcond */
    
    private final native BigInteger getOffset(SafeHandle eventHandle, IntRef errorHandle);
}
