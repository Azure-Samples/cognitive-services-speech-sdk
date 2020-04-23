//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.math.BigInteger;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.StringRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

/**
 * Contains detailed information about result of a recognition operation.
 */
public class RecognitionResult {

    /*! \cond PROTECTED */

    protected RecognitionResult(long result) {

        if (result != 0) {
            this.resultHandle = new SafeHandle(result, SafeHandleType.RecognitionResult);

            StringRef stringVal = new StringRef("");
            Contracts.throwIfFail(getResultId(resultHandle, stringVal));
            this.resultId = stringVal.getValue();
                
            IntRef intVal = new IntRef(0);
            Contracts.throwIfFail(getResultReason(resultHandle, intVal));
            this.reason = ResultReason.values()[(int)intVal.getValue()];
    
            Contracts.throwIfFail(getResultText(resultHandle, stringVal));
            this.text = stringVal.getValue();
    
            this.duration = getResultDuration(resultHandle, intVal);
            Contracts.throwIfFail(intVal.getValue());
    
            this.offset = getResultOffset(resultHandle, intVal);
            Contracts.throwIfFail(intVal.getValue());
    
            IntRef propertyHandle = new IntRef(0);
            Contracts.throwIfFail(getPropertyBagFromResult(resultHandle, propertyHandle));
            this.properties = new PropertyCollection(propertyHandle);
        }
    }

    /*! \endcond */

    /**
     * Specifies the result identifier.
     * @return Specifies the result identifier.
     */
    public String getResultId() {  
        return this.resultId;
    }

    /**
     * Specifies reason the result was created.
     * @return Specifies reason of the result.
     */
    public ResultReason getReason() {
        return this.reason;
    }

    /**
     * Presents the recognized text in the result.
     * @return Presents the recognized text in the result.
     */
    public String getText() {
        return this.text;
    }

    /**
     * Duration of recognized speech in 100nsec increments.
     * @return Duration of recognized speech in 100nsec increments.
     */
    public BigInteger getDuration() {
        return this.duration;
    }

    /**
     * Offset of recognized speech in 100nsec increments.
     * @return Offset of recognized speech in 100nsec increments.
     */
    public BigInteger getOffset() {
        return this.offset;
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
            resultHandle.close();
            this.resultHandle = null;
        }

        if (this.properties != null) {
            this.properties.close();
            this.properties = null;
        }
    }
    
    /*! \cond INTERNAL */
    /**
     * Returns the recognition result implementation.
     * @return The implementation of the result.
     */
    public SafeHandle getImpl() {
        Contracts.throwIfNull(this.resultHandle, "result");
        return this.resultHandle;
    }
    /*! \endcond */

    /*! \cond PROTECTED */
    protected SafeHandle resultHandle = null;
    /*! \endcond */

    private PropertyCollection properties = null;
    private String resultId;
    private ResultReason reason;
    private String text;
    private BigInteger duration;
    private BigInteger offset;

    private final native long getResultId(SafeHandle resultHandle, StringRef resultId);
    private final native long getResultReason(SafeHandle resultHandle, IntRef reasonVal);
    private final native long getResultText(SafeHandle resultHandle, StringRef resultText);
    private final native BigInteger getResultDuration(SafeHandle resultHandle, IntRef errorHandle);
    private final native BigInteger getResultOffset(SafeHandle resultHandle, IntRef errorHandle);
    private final native long getPropertyBagFromResult(SafeHandle resultHandle, IntRef propertyHandle);
}
