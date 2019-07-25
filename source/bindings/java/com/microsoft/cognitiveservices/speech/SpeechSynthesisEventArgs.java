//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines contents of speech synthesis related event.
 * Added in version 1.7.0
 */
public class SpeechSynthesisEventArgs implements Closeable {

    private SpeechSynthesisResult result;

    SpeechSynthesisEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisEventArgs e) {

        Contracts.throwIfNull(e, "e");
        this.result = new SpeechSynthesisResult(e.getResult());
    }

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
        if (this.result != null) {
            this.result.close();
        }
        this.result = null;
    }
}
