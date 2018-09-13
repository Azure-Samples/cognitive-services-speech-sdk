package com.microsoft.cognitiveservices.speech.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
  */
public final class TranslationSynthesisResult
{
    private com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult _resultImpl;

    TranslationSynthesisResult(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult result) {
        Contracts.throwIfNull(result, "result");

        _resultImpl = result;
        _AudioData = null;
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
        return "TranslationSynthesisResult: <<audioData " + (_AudioData != null ? _AudioData.length : "no") + " bytes available>>";
    }
}
