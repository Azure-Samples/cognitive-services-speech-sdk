//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.math.BigInteger;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Contains detailed information about result of a recognition operation.
 */
public class RecognitionResult {
    private String resultId;
    private ResultReason reason;
    private String text;
    private BigInteger duration;
    private BigInteger offset;
    private PropertyCollection properties;
    private com.microsoft.cognitiveservices.speech.internal.RecognitionResult _resultImpl;

    /*! \cond PROTECTED */

    protected RecognitionResult(com.microsoft.cognitiveservices.speech.internal.RecognitionResult result) {
        if(result != null) {
            this._resultImpl = result;
            this.resultId = result.getResultId();
            this.reason = ResultReason.values()[result.getReason().swigValue()];
            this.text = result.getText();
            this.duration = result.Duration();
            this.offset = result.Offset();
            this.properties = new PropertyCollection(result.getProperties());
        }
    }

    /*! \endcond */

    /**
     * Specifies the result identifier.
     * @return Specifies the result identifier.
     */
    public String getResultId() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this.resultId;
    }

    /**
     * Specifies reason the result was created.
     * @return Specifies reason of the result.
     */
    public ResultReason getReason() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this.reason;
    }

    /**
     * Presents the recognized text in the result.
     * @return Presents the recognized text in the result.
     */
    public String getText() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this.text;
    }

    /**
     * Duration of recognized speech in 100nsec increments.
     * @return Duration of recognized speech in 100nsec increments.
     */
    public BigInteger getDuration() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this.duration;
    }

    /**
     * Offset of recognized speech in 100nsec increments.
     * @return Offset of recognized speech in 100nsec increments.
     */
    public BigInteger getOffset() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this.offset;
    }

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public PropertyCollection getProperties() {
        Contracts.throwIfNull(this._resultImpl, "result");
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
     * Returns the recognition result implementation.
     * @return The implementation of the result.
     */
    public com.microsoft.cognitiveservices.speech.internal.RecognitionResult getResultImpl() {
        Contracts.throwIfNull(this._resultImpl, "result");
        return this._resultImpl;
    }
}
