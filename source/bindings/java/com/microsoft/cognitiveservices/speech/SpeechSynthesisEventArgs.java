//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;

/**
 * Defines contents of speech synthesis related event.
 * Added in version 1.7.0
 */
public class SpeechSynthesisEventArgs implements Closeable {

    /*! \cond INTERNAL */

    SpeechSynthesisEventArgs(long eventArgs) {

        Contracts.throwIfNull(eventArgs, "eventArgs");
        eventHandle = new SafeHandle(eventArgs, SafeHandleType.SynthesisEvent);
        IntRef resultRef = new IntRef(0);
        Contracts.throwIfFail(getSynthesisResult(eventHandle, resultRef));
        this.result = new SpeechSynthesisResult(resultRef);
    }
    
    /*! \endcond */

    /**
     * Specifies the speech synthesis result.
     * @return the speech synthesis result.
     */
    public SpeechSynthesisResult getResult() {
        return this.result;
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.eventHandle != null) {
            eventHandle.close();
            this.eventHandle = null;
        }
        if (this.result != null) {
            this.result.close();
            this.result = null;
        }
    }

    private final native long getSynthesisResult(SafeHandle eventHandle, IntRef resultRef);

    private SpeechSynthesisResult result;
    private SafeHandle eventHandle;
}
