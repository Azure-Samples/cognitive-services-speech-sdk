//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXKEYWORDHANDLE = System.IntPtr;

    internal class KeywordRecognitionModel : SpxExceptionThrower, IDisposable
    {
        internal SPXKEYWORDHANDLE keywordHandle = IntPtr.Zero;
        private bool disposed = false;

        internal KeywordRecognitionModel(IntPtr keywordPtr)
        {
            keywordHandle = keywordPtr;
        }

        ~KeywordRecognitionModel()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            if (keywordHandle != IntPtr.Zero)
            {
                ThrowIfFail(keyword_recognition_model_handle_release(keywordHandle));
                keywordHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static KeywordRecognitionModel FromFile(string fileName)
        {
            SPXKEYWORDHANDLE keywordModel = IntPtr.Zero;
            ThrowIfFail(keyword_recognition_model_create_from_file(fileName, out keywordModel));
            return new KeywordRecognitionModel(keywordModel);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool keyword_recognition_model_handle_is_valid(SPXKEYWORDHANDLE hkeyword);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR keyword_recognition_model_handle_release(SPXKEYWORDHANDLE hkeyword);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR keyword_recognition_model_create_from_file(
            [MarshalAs(UnmanagedType.LPStr)] string fileName,
            out SPXKEYWORDHANDLE phkwmodel);
    }
}
