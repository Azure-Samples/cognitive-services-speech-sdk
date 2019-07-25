//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Defines contents of speech synthesis word boundary event.
 * Added in version 1.7.0
 */
public class SpeechSynthesisWordBoundaryEventArgs {

    private long audioOffset;
    private long textOffset;
    private long wordLength;

    SpeechSynthesisWordBoundaryEventArgs(com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisWordBoundaryEventArgs e) {

        Contracts.throwIfNull(e, "e");
        this.audioOffset = e.getAudioOffset().longValue();
        this.textOffset = e.getTextOffset();
        this.wordLength = e.getWordLength();
    }

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
}
