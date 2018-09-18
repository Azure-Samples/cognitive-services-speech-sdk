//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.ResultReason;

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 */
public final class TranslationSynthesisResult
{
    private com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult _resultImpl;
    private ResultReason _reason;

    TranslationSynthesisResult(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult result) {
        Contracts.throwIfNull(result, "result");

        _resultImpl = result;
        _AudioData = null;
        _reason = ResultReason.values()[result.getReason().swigValue()];
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this._resultImpl != null) {
            this._resultImpl.delete();
        }
        this._resultImpl = null;
    }

    /**
      * Specifies reason the result was created.
      * @return Specifies reason of the result.
      */
      public ResultReason getReason() {
        return this._reason;
    }

    /**
      * The voice output of the translated text in the target language.
      * @return Translated text in the target language.
      */
    public byte[] getAudio() {
        if (_AudioData == null) {
            com.microsoft.cognitiveservices.speech.internal.UInt8Vector audio = _resultImpl.getAudio();
            int size = (int)audio.size();
            _AudioData = new byte[size];

            for(int n=0; n<size; n++) {
                _AudioData[n] = (byte)audio.get(n);
            }
        }
        return _AudioData;
    }
    private byte[] _AudioData;

    /**
     * Returns a String that represents the speech recognition result.
     * @return A String that represents the speech recognition result.
     */
    @Override
    public String toString() {
        return "TranslationSynthesisResult" + 
               " Reason:" + this._reason +
               " Audio.length:" + this._AudioData.length +
               ".";
    }
}
