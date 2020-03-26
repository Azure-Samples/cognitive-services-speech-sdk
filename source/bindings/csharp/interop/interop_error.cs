//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXERRORHANDLE = System.IntPtr;

    internal static class SpxError
    {
        public static readonly SPXHR BufferTooSmall = (SPXHR) 0x019;
        public static readonly SPXHR RuntimeError = (SPXHR) 0x01B;
        public static readonly SPXHR InvalidHandle = (SPXHR) 0x021;

        public static string GetMessage(SPXERRORHANDLE errorHandle)
        {
            string message = string.Empty;
            IntPtr pStr = error_get_message(errorHandle);
            if (pStr != IntPtr.Zero)
            {
                message = Utf8StringMarshaler.MarshalNativeToManaged(pStr);
            }
            return message;
        }

        public static string GetCallStack(SPXERRORHANDLE errorHandle)
        {
            string callStack = string.Empty;
            IntPtr pStr = error_get_call_stack(errorHandle);
            if (pStr != IntPtr.Zero)
            {
                callStack = Utf8StringMarshaler.MarshalNativeToManaged(pStr);
            }
            return callStack;
        }

        public static int GetErrorCode(SPXERRORHANDLE errorHandle)
        {
            int errorCode = (int)error_get_error_code(errorHandle);
            return errorCode;
        }

        public static void Release(SPXERRORHANDLE errorHandle)
        {
            SpxExceptionThrower.ThrowIfFail(error_release(errorHandle));
        }

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        private static extern IntPtr error_get_message(SPXERRORHANDLE errorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        private static extern IntPtr error_get_call_stack(SPXERRORHANDLE errorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        private static extern SPXHR error_get_error_code(SPXERRORHANDLE errorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        private static extern SPXHR error_release(SPXERRORHANDLE errorHandle);

    }
}
