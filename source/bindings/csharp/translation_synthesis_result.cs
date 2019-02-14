//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
    /// </summary>
    public sealed class TranslationSynthesisResult
    {
        internal TranslationSynthesisResult(IntPtr resultPtr)
        {
            ThrowIfNull(resultPtr);
            resultHandle = new InteropSafeHandle(resultPtr, Internal.RecognitionResult.recognizer_result_handle_release);

            ResultReason resultReason = ResultReason.NoMatch;
            ThrowIfFail(Internal.RecognitionResult.result_get_reason(resultHandle, ref resultReason));
            this.Reason = resultReason;

            GetAudioData(resultHandle);
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

        private void GetAudioData(InteropSafeHandle resultHandle)
        {
            ThrowIfNull(resultHandle, "Invalid synthesis result handle.");
            if (Internal.RecognitionResult.recognizer_result_handle_is_valid(resultHandle))
            {
                int bufLen = 0;
                IntPtr hr = Internal.RecognitionResult.translation_synthesis_result_get_audio_data(resultHandle, IntPtr.Zero, ref bufLen);
                if (hr == SpxError.BufferTooSmall)
                {
                    IntPtr audioBuffer = Marshal.AllocHGlobal(bufLen);
                    try
                    {
                        ThrowIfFail(Internal.RecognitionResult.translation_synthesis_result_get_audio_data(resultHandle, audioBuffer, ref bufLen));
                        audio = new byte[bufLen];
                        Marshal.Copy(audioBuffer, audio, 0, bufLen);
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(audioBuffer);
                    }
                    return;
                }
                ThrowIfFail(hr);
            }
        }

        private byte[] audio = new byte[0];
        private InteropSafeHandle resultHandle;
    }
}
