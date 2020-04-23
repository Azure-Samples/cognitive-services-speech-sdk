//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.ResultReason;

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 */
public final class TranslationSynthesisResult
{

    /*! \cond INTERNAL */

    TranslationSynthesisResult(long result) {
        Contracts.throwIfNull(result, "result");        
        resultHandle = new SafeHandle(result, SafeHandleType.RecognitionResult);
        IntRef intVal = new IntRef(0);
        Contracts.throwIfFail(getResultReason(resultHandle, intVal));
        this.reason = ResultReason.values()[(int)intVal.getValue()];
        IntRef hr = new IntRef(0);
        audioData = getAudio(resultHandle, hr);
        Contracts.throwIfFail(hr.getValue());
    }

    /*! \endcond */

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.resultHandle != null) {
            this.resultHandle.close();
        }
        this.resultHandle = null;
    }

    /**
     * Specifies reason the result was created.
     * @return Specifies reason of the result.
     */
    public ResultReason getReason() {
        return this.reason;
    }

    /**
      * The voice output of the translated text in the target language.
      * @return Translated text in the target language.
      */
    public byte[] getAudio() {
        return audioData;
    }

    /**
     * Returns a String that represents the speech recognition result.
     * @return A String that represents the speech recognition result.
     */
    @Override
    public String toString() {
        return "TranslationSynthesisResult" + 
               " Reason:" + this.reason +
               " Audio.length:" + this.audioData.length +
               ".";
    }
    
    private final native long getResultReason(SafeHandle resultHandle, IntRef reasonVal);
    private final native byte[] getAudio(SafeHandle resultHandle, IntRef hr);

    private byte[] audioData;
    private SafeHandle resultHandle;
    private ResultReason reason;
}
