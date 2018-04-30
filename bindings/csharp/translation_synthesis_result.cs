//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Globalization;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
    /// </summary>
    public sealed class TranslationSynthesisResult
    {
        internal TranslationSynthesisResult(Internal.TranslationSynthesisResult result)
        {
            resultImpl = result;

            Trace.Assert((int)TranslationSynthesisStatus.Success == (int)Internal.TranslationSynthesisStatus.Success);
            Trace.Assert((int)TranslationSynthesisStatus.SynthesisEnd == (int)Internal.TranslationSynthesisStatus.SynthesisEnd);
            Trace.Assert((int)TranslationSynthesisStatus.Error == (int)Internal.TranslationSynthesisStatus.Error);
            SynthesisStatus = (TranslationSynthesisStatus)result.SynthesisStatus;
            FailureReason = result.FailureReason;

            Audio = new byte[result.Audio.Count];
            result.Audio.CopyTo(Audio);
        }

        /// <summary>
        /// Specifies status of translation synthesis.
        /// </summary>
        public TranslationSynthesisStatus SynthesisStatus { get; }


        /// <summary>
        /// translated text in the target language.
        /// </summary>
        public byte[] Audio { get; }

        /// <summary>
        /// Contains failure reason if TextStatus is Error. Otherwise it is empty.
        /// </summary>
        public string FailureReason { get; }

        /// <summary>
        /// Returns a string that represents the synthesis result.
        /// </summary>
        /// <returns>A string that represents the synthesis result.</returns>
        public override string ToString()
        {
            var text = string.Format(CultureInfo.InvariantCulture, "SynthesisStatus: {0}\n", SynthesisStatus);
            if (SynthesisStatus == TranslationSynthesisStatus.Success)
            {
                string.Format(CultureInfo.InvariantCulture, "AudioSize:{0}.", Audio.Length);
            }
            else if (SynthesisStatus == TranslationSynthesisStatus.SynthesisEnd)
            {
                string.Format(CultureInfo.InvariantCulture, "End of synthesis data.");
            }
            else
            {
                text += string.Format(CultureInfo.InvariantCulture, "Failure reason: {0} \n", FailureReason);
            }
            return text;
        }

        // Hold the referece.
        private Internal.TranslationSynthesisResult resultImpl;
    }
}
