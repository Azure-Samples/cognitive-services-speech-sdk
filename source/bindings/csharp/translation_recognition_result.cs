//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Translation
{
    /// <summary>
    /// Defines a translation result.
    /// </summary>
    public class TranslationRecognitionResult : RecognitionResult
    {
        internal TranslationRecognitionResult(IntPtr resultPtr) : base(resultPtr)
        {
            GetTranslationTexts();
        }

        /// <summary>
        /// Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key
        /// is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
        /// </summary>
        public IReadOnlyDictionary<string, string> Translations { get { return translationTextResultMap; } }

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            var text = string.Format(CultureInfo.InvariantCulture, "ResultId:{0} Reason:{1}, Recognized text:<{2}>.\n", ResultId, Reason, Text);
            foreach (var element in Translations)
            {
                text += $"    Translation in {element.Key}: <{element.Value}>.\n";
            }
            return text;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        private void GetTranslationTexts()
        {
            ThrowIfNull(resultHandle, "Invalid result handle");

            if (Internal.RecognitionResult.recognizer_result_handle_is_valid(resultHandle))
            {
                int bufLen = 0;
                translationTextResultMap = new Dictionary<string, string>();
                IntPtr hr = Internal.RecognitionResult.translation_text_result_get_translation_text_buffer_header(resultHandle, IntPtr.Zero, ref bufLen);
                if (hr == SpxError.BufferTooSmall)
                {
                    IntPtr translationResultBuffer = Marshal.AllocHGlobal(bufLen);
                    try
                    {
                        ThrowIfFail(Internal.RecognitionResult.translation_text_result_get_translation_text_buffer_header(resultHandle, translationResultBuffer, ref bufLen));

                        int offset = 0;
                        int bufferSize = (int)Marshal.ReadIntPtr(translationResultBuffer, offset);

                        ThrowIfFail(bufferSize == bufLen, SpxError.RuntimeError.ToInt32());

                        offset += IntPtr.Size;
                        int numberEntries = (int)Marshal.ReadIntPtr(translationResultBuffer, offset);

                        offset += IntPtr.Size;
                        IntPtr targetLanguagesPtr = Marshal.ReadIntPtr(translationResultBuffer, offset);

                        offset += IntPtr.Size;
                        IntPtr translationTextsPtr = Marshal.ReadIntPtr(translationResultBuffer, offset);

                        // read targetLanguages and translationTexts
                        for (int entryIndex = 0; entryIndex < numberEntries; entryIndex++)
                        {
                            offset = IntPtr.Size * entryIndex;
                            IntPtr targetLanguageStrPtr = Marshal.ReadIntPtr(targetLanguagesPtr, offset);
                            string language = Utf8StringMarshaler.MarshalNativeToManaged(targetLanguageStrPtr);
                            IntPtr translationTextStrPtr = Marshal.ReadIntPtr(translationTextsPtr, offset);
                            string text = Utf8StringMarshaler.MarshalNativeToManaged(translationTextStrPtr);
                            translationTextResultMap[language] = text;
                        }
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(translationResultBuffer);
                    }
                }
            }
        }

        private Dictionary<string, string> translationTextResultMap;
    }
}
