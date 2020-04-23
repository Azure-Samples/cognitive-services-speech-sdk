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
 * Defines contents of speech synthesis word boundary event.
 * Added in version 1.7.0
 */
public class SpeechSynthesisWordBoundaryEventArgs {

    /*! \cond INTERNAL */

    SpeechSynthesisWordBoundaryEventArgs(long eventArgs) {
        Contracts.throwIfNull(eventArgs, "eventArgs");
        
        SafeHandle eventHandle = new SafeHandle(eventArgs, SafeHandleType.SynthesisEvent);
        IntRef audioOffsetRef = new IntRef(0);
        IntRef textOffsetRef = new IntRef(0);
        IntRef wordLengthRef = new IntRef(0);
        Contracts.throwIfFail(getWordBoundaryEventValues(eventHandle, audioOffsetRef, textOffsetRef, wordLengthRef));
        this.audioOffset = audioOffsetRef.getValue();
        this.textOffset = textOffsetRef.getValue();
        this.wordLength = wordLengthRef.getValue();
        eventHandle.close();
    }

    /*! \endcond */
    
    /**
     * Specifies current word's binary offset in output audio, by ticks (100ns).
     * @return Current word's binary offset in output audio, by ticks (100ns).
     */
    public long getAudioOffset() {
        return this.audioOffset;
    }

    /**
     * Specifies current word's text offset in input text, by characters.
     * @return Current word's text offset in input text, by characters.
     */
    public long getTextOffset() {
        return this.textOffset;
    }

    /**
     * Specifies current word's length, by characters.
     * @return Current word's length, by characters.
     */
    public long getWordLength() {
        return this.wordLength;
    }

    private final native long getWordBoundaryEventValues(SafeHandle eventHandle, IntRef audioOffset, IntRef textOffset, IntRef wordLength);

    private long audioOffset;
    private long textOffset;
    private long wordLength;

}
