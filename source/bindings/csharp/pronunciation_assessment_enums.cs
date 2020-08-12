//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech.PronunciationAssessment
{
    /// <summary>
    /// Defines the point system for pronunciation score calibration; default value is FivePoint.
    /// Added in version 1.14.0
    /// </summary>
    public enum GradingSystem
    {
        /// <summary>
        /// Five point calibration
        /// </summary>
        FivePoint = 1,

        /// <summary>
        /// Hundred mark
        /// </summary>
        HundredMark = 2
    }

    /// <summary>
    /// Defines the pronunciation evaluation granularity; default value is Phoneme.
    /// Added in version 1.14.0
    /// </summary>
    public enum Granularity
    {
        /// <summary>
        /// Shows the score on the full text, word and phoneme level
        /// </summary>
        Phoneme = 1,

        /// <summary>
        /// Shows the score on the full text and word level
        /// </summary>
        Word = 2,

        /// <summary>
        /// Shows the score on the full text level only
        /// </summary>
        FullText = 3
    }
}
