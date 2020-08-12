//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Globalization;
using System.Linq;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.PronunciationAssessment
{
    /// <summary>
    /// Contains pronunciation assessment result
    /// Added in 1.14.0
    /// </summary>
    public class PronunciationAssessmentResult
    {
        /// <summary>
        /// The score indicating the pronunciation accuracy of the given speech, which indcates
        /// how closely the phonemes match a native speaker's pronunciation
        /// </summary>
        public double AccuracyScore { get; }

        /// <summary>
        /// The overall score indicating the pronunciation quality of the given speech.
        /// This is calculated from AccuracyScore, FluencyScore and CompletenessScore with weight.
        /// </summary>
        public double PronunciationScore { get; }

        /// <summary>
        /// The score indicating the completeness of the given speech by calculating the ratio of pronounced words towards entire input.
        /// </summary>
        public double CompletenessScore { get; }

        /// <summary>
        /// The score indicating the fluency of the given speech.
        /// </summary>
        public double FluencyScore { get; }

        /// <summary>
        /// Word level pronunciation assessment result
        /// </summary>
        public IEnumerable<PronunciationAssessmentWordResult> Words { get; }

        /// <summary>
        /// Creates an instance of PronunciationAssessmentResult object for the speech recognition result.
        /// </summary>
        /// <param name="result">The speech recongition result.</param>
        /// <returns>A new PronunciationAssessmentResult instance</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "Validated in internal constructor.")]
        public static PronunciationAssessmentResult FromResult(SpeechRecognitionResult result)
        {
            ThrowIfNull(result);
            var detailResult = result.Best();
            if (!detailResult.Any() || detailResult.First().PronunciationAssessment == null)
            {
                return null;
            }
            return new PronunciationAssessmentResult(detailResult.First());
        }

        /// <summary>
        /// Creates an instance of PronunciationAssessmentResult object for a DetailedSpeechRecognitionResult result.
        /// </summary>
        /// <param name="result">The DetailedSpeechRecognitionResult result.</param>
        /// <returns>A new PronunciationAssessmentResult instance</returns>
        internal PronunciationAssessmentResult(DetailedSpeechRecognitionResult result)
        {
            ThrowIfNull(result);
            AccuracyScore = result.PronunciationAssessment.AccuracyScore;
            PronunciationScore = result.PronunciationAssessment.PronunciationScore;
            CompletenessScore = result.PronunciationAssessment.CompletenessScore;
            FluencyScore = result.PronunciationAssessment.FluencyScore;

            if (result.Words == null)
            {
                Words = new List<PronunciationAssessmentWordResult>();
            }
            else
            {
                Words = result.Words.Select(x => PronunciationAssessmentWordResult.FromResult(x))
                    .OfType<PronunciationAssessmentWordResult>().ToList();
            }
        }
    }

    /// <summary>
    /// Contains word level pronunciation assessment result
    /// Added in 1.14.0
    /// </summary>
    public class PronunciationAssessmentWordResult
    {
        /// <summary>
        /// The word text
        /// </summary>
        public string Word { get; }

        /// <summary>
        /// The score indicating the pronunciation accuracy of the given speech, which indicates
        /// how closely the phonemes match a native speaker's pronunciation
        /// </summary>
        public double AccuracyScore { get; }

        /// <summary>
        /// This value indicates whether a word is omitted, inserted or badly pronounced, compared to ReferenceText.
        /// Possible values are None (meaning no error on this word), Omission, Insertion and Mispronunciation.
        /// </summary>
        public string ErrorType { get; }

        /// <summary>
        /// Phoneme level pronunciation assessment result
        /// </summary>
        public IEnumerable<PronunciationAssessmentPhonemeResult> Phonemes { get; }

        internal PronunciationAssessmentWordResult(WordLevelTimingResult result)
        {
            Word = result.Word;
            AccuracyScore = result.PronunciationAssessment.AccuracyScore;
            ErrorType = result.PronunciationAssessment.ErrorType;

            if (result.Phonemes == null)
            {
                Phonemes = new List<PronunciationAssessmentPhonemeResult>();
            }
            else
            {
                Phonemes = result.Phonemes.Select(x => PronunciationAssessmentPhonemeResult.FromResult(x))
                   .OfType<PronunciationAssessmentPhonemeResult>().ToList();
            }
        }

        internal static PronunciationAssessmentWordResult FromResult(WordLevelTimingResult result)
        {
            if (result.PronunciationAssessment == null)
            {
                return null;
            }
            return new PronunciationAssessmentWordResult(result);
        }
    }

    /// <summary>
    /// Contains phoneme level pronunciation assessment result
    /// Added in 1.14.0
    /// </summary>
    public class PronunciationAssessmentPhonemeResult
    {
        /// <summary>
        /// The phoneme text
        /// </summary>
        public string Phoneme { get; }

        /// <summary>
        /// The score indicating the pronunciation accuracy of the given speech, which indicates
        /// how closely the phonemes match a native speaker's pronunciation
        /// </summary>
        public double AccuracyScore { get; }

        internal PronunciationAssessmentPhonemeResult(PhonemeLevelTimingResult result)
        {
            Phoneme = result.Phoneme;
            AccuracyScore = result.PronunciationAssessment.AccuracyScore;
        }

        internal static PronunciationAssessmentPhonemeResult FromResult(PhonemeLevelTimingResult result)
        {
            if (result.PronunciationAssessment == null)
            {
                return null;
            }
            return new PronunciationAssessmentPhonemeResult(result);
        }
    }
}
