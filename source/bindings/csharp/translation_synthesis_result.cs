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

            Reason = (ResultReason)((int)result.Reason);

            audio = new byte[result.Audio.Length];
            result.Audio.CopyTo(audio, 0);
        }

        /// <summary>
        /// Indicates the possible reasons a TranslationSynthesisResult might be generated.
        /// </summary>
        public ResultReason Reason { get; }

        /// <summary>
        /// The voice output of the translated text in the target language.
        /// </summary>
        /// <returns>Synthesized audio data.</returns>
        public byte[] GetAudio() { return (byte[])audio.Clone(); }

        /// <summary>
        /// Returns a string that represents the synthesis result.
        /// </summary>
        /// <returns>A string that represents the synthesis result.</returns>
        public override string ToString()
        {
            var text = string.Format(CultureInfo.InvariantCulture, "Reason: {0}; AudioSize:{1}", Reason, audio.Length);
            if (Reason == ResultReason.SynthesizingAudioCompleted)
            {
                text += string.Format(CultureInfo.InvariantCulture, " (end of synthesis data)");
            }
            return text;
        }

        // Hold the referece.
        private Internal.TranslationSynthesisResult resultImpl;
        private byte[] audio;
    }
}
