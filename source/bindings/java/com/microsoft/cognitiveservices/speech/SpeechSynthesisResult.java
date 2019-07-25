//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Contains detailed information about result of a speech synthesis operation.
 * Added in version 1.7.0
 */
public class SpeechSynthesisResult implements Closeable {
    private String resultId;
    private ResultReason reason;
    private byte[] audioData;
    private PropertyCollection properties;
    private com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisResult _resultImpl;

    /*! \cond PROTECTED */

    protected SpeechSynthesisResult(com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisResult result) {
        Contracts.throwIfNull(result, "result");

        this._resultImpl = result;
        this.resultId = result.getResultId();
        this.reason = ResultReason.values()[result.getReason().swigValue()];
        this.audioData = null;
        this.properties = new PropertyCollection(result.getProperties());
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
      * Gets the length of synthesized audio from the result.
      * @return Length of synthesized audio.
      */
    public long getAudioLength() {
        return _resultImpl.GetAudioLength();
    }

    /**
      * Gets the synthesized audio from the result.
      * @return Synthesized audio.
      */
    public byte[] getAudioData() {
        if (audioData == null) {
            com.microsoft.cognitiveservices.speech.internal.UInt8Vector audio = _resultImpl.GetAudioData();
            int size = (int)audio.size();
            audioData = new byte[size];

            for(int n=0; n<size; n++) {
                audioData[n] = (byte)audio.get(n);
            }
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
        if (this._resultImpl != null) {
            this._resultImpl.delete();
        }
        this._resultImpl = null;

        if (this.properties != null) {
            this.properties.close();
        }
        this.properties = null;
    }

    /**
     * Returns the synthesis result implementation.
     * @return The implementation of the result.
     */
    public com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisResult getResultImpl() {
        return this._resultImpl;
    }
}
