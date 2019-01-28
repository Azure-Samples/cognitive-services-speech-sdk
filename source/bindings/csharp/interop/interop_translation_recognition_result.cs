//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class TranslationRecognitionResult : RecognitionResult
    {

        internal TranslationRecognitionResult(IntPtr resultPtr) : base(resultPtr)
        {
            GetTranslationTexts();
        }

        ~TranslationRecognitionResult()
        {
            Dispose(false);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            disposed = true;
            base.Dispose(disposing);
        }

        public Dictionary<string, string> Translations { get; private set; } = null;

        private void GetTranslationTexts()
        {
            if (recognizer_result_handle_is_valid(resultHandle))
            {
                int bufLen = 0;
                Translations = new Dictionary<string, string>();
                SPXHR hr = translation_text_result_get_translation_text_buffer_header(resultHandle, IntPtr.Zero, ref bufLen);
                if (hr == SpxError.BufferTooSmall)
                {
                    IntPtr translationResultBuffer = Marshal.AllocHGlobal(bufLen);
                    try
                    {
                        ThrowIfFail(translation_text_result_get_translation_text_buffer_header(resultHandle, translationResultBuffer, ref bufLen));

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
                            Translations[language] = text;
                        }
                    }
                    finally
                    {
                        Marshal.FreeHGlobal(translationResultBuffer);
                    }
                }
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR translation_text_result_get_translation_text_buffer_header(SPXRESULTHANDLE handle, IntPtr translationResultBuffer, ref int lengthPointer);
    }
}
