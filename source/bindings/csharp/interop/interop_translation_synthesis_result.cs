//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class TranslationSynthesisResult : RecognitionResult
    {
        internal TranslationSynthesisResult(IntPtr resultPtr) : base(resultPtr)
        {
            GetAudioData(resultHandle);
        }

        ~TranslationSynthesisResult()
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

        public Byte[] Audio { get; private set; } = new byte[0];

        private void GetAudioData(SPXRESULTHANDLE resultHandle)
        {
            if (recognizer_result_handle_is_valid(resultHandle))
            {
                int bufLen = 0;
                SPXHR hr = translation_synthesis_result_get_audio_data(resultHandle, IntPtr.Zero, ref bufLen);
                if (hr == SpxError.BufferTooSmall)
                {
                    IntPtr audioBuffer = Marshal.AllocHGlobal(bufLen);
                    try
                    {
                        ThrowIfFail(translation_synthesis_result_get_audio_data(resultHandle, audioBuffer, ref bufLen));
                        Audio = new byte[bufLen];
                        Marshal.Copy(audioBuffer, Audio, 0, bufLen);
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

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR translation_synthesis_result_get_audio_data(SPXRESULTHANDLE handle, IntPtr audioBuffer, ref int lengthPointer);

    }

}
