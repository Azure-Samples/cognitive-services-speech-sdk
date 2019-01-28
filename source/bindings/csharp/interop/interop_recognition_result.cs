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
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal class RecognitionResult : SpxExceptionThrower, IDisposable
    {
        internal SPXRESULTHANDLE resultHandle = IntPtr.Zero;
        protected const Int32 maxCharCount = 1024;
        protected bool disposed = false;

        internal RecognitionResult(IntPtr resultHandlePtr)
        {
            ThrowIfNull(resultHandlePtr);
            resultHandle = resultHandlePtr;
            SPXPROPERTYBAGHANDLE propertyHandle = IntPtr.Zero;
            ThrowIfFail(result_get_property_bag(resultHandlePtr, out propertyHandle));
            Properties = new PropertyCollection(propertyHandle);
        }

        ~RecognitionResult()
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

            if (Properties != null)
            {
                Properties.Dispose();
            }

            if (resultHandle != IntPtr.Zero)
            {
                LogErrorIfFail(recognizer_result_handle_release(resultHandle));
                resultHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public string ResultId
        {
            get
            {
                return SpxFactory.GetDataFromHandleUsingDelegate(result_get_result_id, resultHandle, maxCharCount);
            }
        }

        public ResultReason Reason
        {
            get
            {
                ThrowIfNull(resultHandle);
                ResultReason resultReason = ResultReason.NoMatch;
                ThrowIfFail(result_get_reason(resultHandle, ref resultReason));
                return resultReason;
            }
        }

        public string Text
        {
            get
            {
                ThrowIfNull(resultHandle);
                return SpxFactory.GetDataFromHandleUsingDelegate(result_get_text, resultHandle, maxCharCount);
            }
        }

        public ulong Duration
        {
            get
            {
                ThrowIfNull(resultHandle);
                UInt64 duration = 0;
                ThrowIfFail(result_get_duration(resultHandle, ref duration));
                return duration;
            }
        }

        public ulong Offset
        {
            get
            {
                ThrowIfNull(resultHandle);
                UInt64 offset = 0;
                ThrowIfFail(result_get_offset(resultHandle, ref offset));
                return offset;
            }
        }

        public PropertyCollection Properties { get; } = null;

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_reason(SPXRESULTHANDLE hresult, ref ResultReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_reason_canceled(SPXRESULTHANDLE hresult, ref CancellationReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_canceled_error_code(SPXRESULTHANDLE hresult, ref CancellationErrorCode errorCode);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR result_get_result_id(SPXRESULTHANDLE hresult, IntPtr pszResultId, UInt32 cchResultId);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR result_get_text(SPXRESULTHANDLE hresult, IntPtr pszText, UInt32 cchText);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_offset(SPXRESULTHANDLE hresult, ref UInt64 offset);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_duration(SPXRESULTHANDLE hresult, ref UInt64 duration);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR result_get_property_bag(SPXRESULTHANDLE hresult, out SPXPROPERTYBAGHANDLE hpropbag);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_result_handle_is_valid(SPXRESULTHANDLE hresult);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_result_handle_release(SPXRESULTHANDLE hresult);

    }
}
