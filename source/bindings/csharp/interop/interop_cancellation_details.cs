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

    internal class CancellationDetails : SpxExceptionThrower, IDisposable
    {
        private RecognitionResult recognitionResult = null;
        private bool disposed = false;

        internal CancellationDetails(RecognitionResult result)
        {
            ThrowIfNull(result);
            recognitionResult = result;
        }

        ~CancellationDetails()
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

        public static CancellationDetails FromResult(RecognitionResult result)
        {
            return new CancellationDetails(result);
        }

        public CancellationReason Reason
        {
            get
            {
                CancellationReason reason;
                ThrowIfFail(result_get_reason_canceled(recognitionResult.resultHandle, out reason));
                return reason;
            }
        }

        public CancellationErrorCode ErrorCode
        {
            get
            {
                CancellationErrorCode code;
                ThrowIfFail(result_get_canceled_error_code(recognitionResult.resultHandle, out code));
                return code;
            }
        }

        public string ErrorDetails
        {
            get
            {
                return recognitionResult.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails);
            }
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_reason_canceled(SPXRESULTHANDLE hresult, out CancellationReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_canceled_error_code(SPXRESULTHANDLE hresult, out CancellationErrorCode errorCode);
    }
}
