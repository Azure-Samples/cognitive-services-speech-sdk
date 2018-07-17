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
    TranslationSynthesisResult(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult result)
    {
        Contracts.throwIfNull(result, "result");

        _result = result;
        _AudioData = null;

        com.microsoft.cognitiveservices.speech.internal.SynthesisStatusCode status = result.getSynthesisStatus();
        if (com.microsoft.cognitiveservices.speech.internal.SynthesisStatusCode.Success == status) {
            _synthesisStatus = SynthesisStatus.Success;
        }
        else if (com.microsoft.cognitiveservices.speech.internal.SynthesisStatusCode.SynthesisEnd == status) {
            _synthesisStatus = SynthesisStatus.SynthesisEnd;
        }
        else if (com.microsoft.cognitiveservices.speech.internal.SynthesisStatusCode.Error == status) {
            _synthesisStatus = SynthesisStatus.Error;
        }
        else {
            throw new IllegalArgumentException("unexpected status");
        }
    }

    /**
      * Translated text in the target language.
      * @return Translated text in the target language.
      */
    public byte[] getAudio()
    {
        if (_AudioData == null) {
            com.microsoft.cognitiveservices.speech.internal.UInt8Vector audio = _result.getAudio();
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
      * Status of the synthesis.
      * @return status of the synthesis.
      */
    public SynthesisStatus getSynthesisStatus() {
        return _synthesisStatus;
    }
    SynthesisStatus _synthesisStatus;

    /**
      * Returns a String that represents the speech recognition result.
      * @return A String that represents the speech recognition result.
      */
    @Override
    public String toString()
    {
        return "Status <<" + getSynthesisStatus() + ", audioData " + (_AudioData != null ? _AudioData.length : "no") + " bytes available>>";
    }

    private com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult _result;
}
