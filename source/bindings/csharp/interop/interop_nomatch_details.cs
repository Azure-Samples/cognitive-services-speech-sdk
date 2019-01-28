//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class NoMatchDetails : SpxExceptionThrower, IDisposable
    {
        private RecognitionResult recognitionResult = null;
        private bool disposed = false;

        internal NoMatchDetails(RecognitionResult result)
        {
            ThrowIfNull(result);
            recognitionResult = result;
        }

        ~NoMatchDetails()
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
                if (recognitionResult != null)
                {
                    recognitionResult.Dispose();
                    recognitionResult = null;
                }
            }
            // dispose unmanaged resources
            disposed = true;
        }

        public static NoMatchDetails FromResult(RecognitionResult result)
        {
            return new NoMatchDetails(result);
        }

        public NoMatchReason Reason
        {
            get
            {
                NoMatchReason reason = NoMatchReason.NotRecognized;
                ThrowIfFail(result_get_no_match_reason(recognitionResult.resultHandle, ref reason));
                return reason;
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_no_match_reason(SPXRESULTHANDLE hresult, ref NoMatchReason reason);
    }
}
