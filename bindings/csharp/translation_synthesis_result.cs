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
            Trace.Assert((int)SynthesisStatus.Success == (int)Internal.SynthesisStatusCode.Success);
            Trace.Assert((int)SynthesisStatus.SynthesisEnd == (int)Internal.SynthesisStatusCode.SynthesisEnd);
            Trace.Assert((int)SynthesisStatus.Error == (int)Internal.SynthesisStatusCode.Error);
            Status = (SynthesisStatus)result.SynthesisStatus;

            resultImpl = result;
            FailureReason = result.FailureReason;

            Audio = new byte[result.Audio.Count];
            result.Audio.CopyTo(Audio);
        }

        /// <summary>
        /// Specifies status of translation synthesis.
        /// </summary>
        public SynthesisStatus Status { get; }


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
            var text = string.Format(CultureInfo.InvariantCulture, "SynthesisStatus: {0}\n", Status);
            if (Status == SynthesisStatus.Success)
            {
                string.Format(CultureInfo.InvariantCulture, "AudioSize:{0}.", Audio.Length);
            }
            else if (Status == SynthesisStatus.SynthesisEnd)
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
