//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.PronunciationAssessment
{
    /// <summary>
    /// Class that defines pronunciation assessment configuration
    /// Added in 1.14.0
    /// </summary>
    public class PronunciationAssessmentConfig
    {
        internal InteropSafeHandle configHandle;
        internal PropertyCollection progBag = null;

        internal PronunciationAssessmentConfig(IntPtr configPtr)
        {
            Setup(configPtr);
        }

        /// <summary>
        /// Creates an instance of the PronunciationAssessmentConfig
        /// For the parameters details, see
        /// https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
        /// </summary>
        /// <param name="referenceText">The reference text</param>
        /// <param name="gradingSystem">The point system for score calibration</param>
        /// <param name="granularity">The evaluation granularity</param>
        /// <param name="enableMiscue">If enables miscue calculation</param>
        /// <returns>A new PronunciationAssessmentConfig instance.</returns>
        public PronunciationAssessmentConfig(string referenceText,
            GradingSystem gradingSystem = GradingSystem.FivePoint,
            Granularity granularity = Granularity.Phoneme,
            bool enableMiscue = false)
        {
            IntPtr pronunciationAssessmentConfigHandle = IntPtr.Zero;
            IntPtr referenceTexTPtr = Utf8StringMarshaler.MarshalManagedToNative(referenceText);
            ThrowIfFail(Internal.PronunciationAssessmentConfig.create_pronunciation_assessment_config(out pronunciationAssessmentConfigHandle, referenceTexTPtr, (int)gradingSystem, (int)granularity, enableMiscue));
            Setup(pronunciationAssessmentConfigHandle);
        }

        internal void Setup(IntPtr configPtr)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.PronunciationAssessmentConfig.pronunciation_assessment_config_release);
            IntPtr hpropbag = IntPtr.Zero;
            ThrowIfFail(Internal.PronunciationAssessmentConfig.pronunciation_assessment_config_get_property_bag(configHandle, out hpropbag));
            progBag = new PropertyCollection(hpropbag);
        }

        /// <summary>
        /// Creates an instance of the PronunciationAssessmentConfig from json.
        /// see https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
        /// </summary>
        /// <param name="json">The json string containing the pronunciation assessment parameters.</param>
        /// <returns>A new PronunciationAssessmentConfig instance.</returns>
        public static PronunciationAssessmentConfig FromJson(string json)
        {
            IntPtr pronunciationAssessmentConfigHandle = IntPtr.Zero;
            IntPtr jsonPtr = Utf8StringMarshaler.MarshalManagedToNative(json);
            ThrowIfFail(Internal.PronunciationAssessmentConfig.create_pronunciation_assessment_config_from_json(out pronunciationAssessmentConfigHandle, jsonPtr));
            return new PronunciationAssessmentConfig(pronunciationAssessmentConfigHandle);
        }

        /// <summary>
        /// Reference Text
        /// </summary>
        public string ReferenceText
        {
            get
            {
                return progBag.GetProperty(PropertyId.PronunciationAssessment_ReferenceText);
            }

            set
            {
                progBag.SetProperty(PropertyId.PronunciationAssessment_ReferenceText, value);
            }
        }

        /// <summary>
        /// Gets and Sets the scenario ID, which is a GUID indicating a customized point system.
        /// </summary>
        public string ScenarioId
        {
            get
            {
                return progBag.GetProperty(PropertyId.PronunciationAssessment_ScenarioId);
            }

            set
            {
                progBag.SetProperty(PropertyId.PronunciationAssessment_ScenarioId, value);
            }
        }

        /// <summary>
        /// Gets to json string of pronunciation assessment parameters.
        /// </summary>
        /// <returns>json string of pronunciation assessment parameters.</returns>
        public string ToJson()
        {
            ThrowIfNull(configHandle);
            string jsonVal = string.Empty;
            IntPtr pStr = Internal.PronunciationAssessmentConfig.pronunciation_assessment_config_to_json(configHandle);
            if (pStr != IntPtr.Zero)
            {
                try
                {
                    jsonVal = Utf8StringMarshaler.MarshalNativeToManaged(pStr);
                }
                finally
                {
                    ThrowIfFail(Internal.PropertyCollection.property_bag_free_string(pStr));
                }
            }

            return jsonVal;
        }

        /// <summary>
        /// Applies the settings in this config to a recognizer.
        /// </summary>
        /// <param name="recognizer">The target recognizer.</param>
        public void ApplyTo(Recognizer recognizer)
        {
            ThrowIfNull(configHandle);
            ThrowIfNull(recognizer);
            ThrowIfNull(recognizer.recoHandle);
            ThrowIfNull(Internal.PronunciationAssessmentConfig.pronunciation_assessment_config_apply_to_recognizer(configHandle, recognizer.recoHandle));
        }
    }
}
