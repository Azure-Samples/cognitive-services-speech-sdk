//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.ArrayList;
import java.util.List;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGradingSystem;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGranularity;
import com.microsoft.cognitiveservices.speech.Recognizer;

/**
 * Represents pronunciation assessment configuration
 * Added in version 1.14.0
 */
public final class PronunciationAssessmentConfig implements Closeable
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /**
     * Initializes an instance of the PronunciationAssessmentConfig
     * For the parameters details, see
     * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
     * @param referenceText The reference text
     * @param gradingSystem The point system for score calibration
     * @param granularity The evaluation granularity
     * @param enableMiscue If enables miscue calculation
     */
    public PronunciationAssessmentConfig(String referenceText, PronunciationAssessmentGradingSystem gradingSystem, PronunciationAssessmentGranularity granularity, boolean enableMiscue) {
        IntRef pronAssessmentConfigRef = new IntRef(0);
        Contracts.throwIfFail(create(pronAssessmentConfigRef, referenceText, gradingSystem.getValue(), granularity.getValue(), enableMiscue));
        init(pronAssessmentConfigRef.getValue());
    }

    /**
     * Initializes an instance of the PronunciationAssessmentConfig
     * For the parameters details, see
     * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
     * @param referenceText The reference text
     * @param gradingSystem The point system for score calibration
     * @param granularity The evaluation granularity
     */
    public PronunciationAssessmentConfig(String referenceText, PronunciationAssessmentGradingSystem gradingSystem, PronunciationAssessmentGranularity granularity) {
        this(referenceText, gradingSystem, granularity, false);
    }

    /**
     * Initializes an instance of the PronunciationAssessmentConfig
     * For the parameters details, see
     * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
     * @param referenceText The reference text
     */
    public PronunciationAssessmentConfig(String referenceText) {
        this(referenceText, PronunciationAssessmentGradingSystem.FivePoint, PronunciationAssessmentGranularity.Phoneme);
    }

    /**
     * Creates an instance of the PronunciationAssessmentConfig from a json string
     * @param json the json string
     * @return The pronunciation assessment configuration being created.
     */
    public static PronunciationAssessmentConfig fromJson(String json) {
        IntRef pronAssessmentConfigRef = new IntRef(0);
        Contracts.throwIfFail(fromJson(pronAssessmentConfigRef, json));
        return new PronunciationAssessmentConfig(pronAssessmentConfigRef.getValue());
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }
        if (propertyHandle != null)
        {
            propertyHandle.close();
            propertyHandle = null;
        }
        if (configHandle != null){
            this.configHandle.close();
            this.configHandle = null;
        }
        disposed = true;
    }

    /**
     * Sets the reference text.
     * @param value the reference text.
     */
    public void setReferenceText(String value) {
        Contracts.throwIfNull(value, "value");
        propertyHandle.setProperty(PropertyId.PronunciationAssessment_ReferenceText, value);
    }

    /**
     * Gets the reference text.
     * @return The reference text.
     */
    public String getReferenceText() {
        return propertyHandle.getProperty(PropertyId.PronunciationAssessment_ReferenceText);
    }

    /**
     * Gets the json string of pronunciation assessment parameters.
     * @return json string of pronunciation assessment parameters.
     */
    public String toJson() {
        Contracts.throwIfNull(configHandle, "configHandle");
        return toJson(configHandle);
    }

    /**
     * Applies the settings in this config to a recognizer.
     * @param recognizer The target recognizer.
     */
    public void applyTo(Recognizer recognizer)
    {
        Contracts.throwIfNull(configHandle, "configHandle");
        Contracts.throwIfNull(recognizer, "recognizer");
        Contracts.throwIfNull(recognizer.getImpl(), "recoHandle");
        applyTo(configHandle, recognizer.getImpl());
    }

    /*! \cond INTERNAL */
    /**
     * Returns the PronunciationAssessmentConfig
     * @return The implementation of the PronunciationAssessmentConfig object
     */
    public SafeHandle getImpl()
    {
        return configHandle;
    }
    /*! \endcond */

    private PronunciationAssessmentConfig(long handleValue) {
        init(handleValue);
    }

    private void init(long handleValue) {
        Contracts.throwIfNull(handleValue, "handleValue");
        this.configHandle = new SafeHandle(handleValue, SafeHandleType.PronunciationAssessmentConfig);
        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBag(configHandle, propHandle));
        this.propertyHandle = new PropertyCollection(propHandle);
    }

    private final static native long create(IntRef pronConfigRef, String referenceText, int gradingSystem, int granularity, boolean enableMiscue);
    private final static native long fromJson(IntRef pronConfigRef, String json);
    private final native long getPropertyBag(SafeHandle pronConfigRef, IntRef propHandle);
    private final native long applyTo(SafeHandle pronConfigRef, SafeHandle recoHandle);
    private final native String toJson(SafeHandle pronConfigRef);

    private SafeHandle configHandle = null;
    private PropertyCollection propertyHandle = null;
    private boolean disposed = false;
}
