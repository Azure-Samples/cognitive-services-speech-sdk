//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

/**
 * Contains detailed information about result of a speech synthesis operation.
 * Added in version 1.7.0
 */
public class SpeechSynthesisResult implements Closeable {

    /*! \cond PROTECTED */

    protected SpeechSynthesisResult(IntRef result) {
        Contracts.throwIfNull(result, "result");

        this.resultHandle = new SafeHandle(result.getValue(), SafeHandleType.SynthesisResult);

        StringRef stringRef = new StringRef("");
        Contracts.throwIfFail(getResultId(resultHandle, stringRef));
        this.resultId = stringRef.getValue();

        IntRef intRef = new IntRef(0);
        Contracts.throwIfFail(getResultReason(resultHandle, intRef));
        this.reason = ResultReason.values()[(int)intRef.getValue()];

        this.audioData = null;

        IntRef propertyRef = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromResult(resultHandle, propertyRef));
        this.properties = new PropertyCollection(propertyRef);
    }

    /*! \endcond */

    /**
     * Gets the unique ID of the result.
     * @return Specifies unique ID of the result.
     */
    public String getResultId() {
        return this.resultId;
    }

    /**
     * Gets the reason the result was created.
     * @return Specifies reason of the result.
     */
    public ResultReason getReason() {
        return this.reason;
    }

    /**
      * Gets the length of synthesized audio in bytes.
      * @return Length of synthesized audio.
      */
    public long getAudioLength() {
        IntRef audioLengthRef = new IntRef(0);
        Contracts.throwIfFail(getAudioLength(resultHandle, audioLengthRef));
        return audioLengthRef.getValue();
    }

    /**
      * Gets the synthesized audio from the result.
      * @return Synthesized audio.
      */
    public byte[] getAudioData() {
        if (audioData == null) {
            IntRef hr = new IntRef(0);
            audioData = getAudio(resultHandle, hr);
            Contracts.throwIfFail(hr.getValue());
        }
        return audioData;
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public PropertyCollection getProperties() {
        return this.properties;
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    public void close() {
        if (this.resultHandle != null) {
            this.resultHandle.close();
            this.resultHandle = null;
        }

        if (this.properties != null) {
            this.properties.close();
            this.properties = null;
        }
    }

    /*! \cond INTERNAL */

    /**
     * Returns the synthesis result implementation.
     * @return The implementation of the result.
     */
    public SafeHandle getImpl() {
        return this.resultHandle;
    }

    /*! \endcond */
    
    private final native long getResultId(SafeHandle resultHandle, StringRef propertyRef);
    private final native long getResultReason(SafeHandle resultHandle, IntRef propertyRef);
    private final native long getPropertyBagFromResult(SafeHandle resultHandle, IntRef propertyRef);
    private final native long getAudioLength(SafeHandle resultHandle, IntRef audioLengthRef);    
    private final native byte[] getAudio(SafeHandle resultHandle, IntRef hr);

    private String resultId;
    private ResultReason reason;
    private byte[] audioData;
    private PropertyCollection properties;
    private SafeHandle resultHandle = null;
}
