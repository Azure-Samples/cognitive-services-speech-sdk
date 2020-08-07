//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Test.Internal
{
    using SPXHR = System.IntPtr;
    using SPXERRORHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogMessageCallbackFunctionDelegate([MarshalAs(UnmanagedType.LPStr)] string logMessage);

    internal class Diagnostics
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR diagnostics_logmessage_set_callback(LogMessageCallbackFunctionDelegate callback);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR diagnostics_logmessage_set_filters([MarshalAs(UnmanagedType.LPStr)] string filters);
    }

    internal static class Import
    {
#if IOS
        public const string NativeDllName = "__Internal";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#elif OSX
        public const string NativeDllName = "libMicrosoft.CognitiveServices.Speech.core.dylib";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#elif UNIX
        public const string NativeDllName = "libMicrosoft.CognitiveServices.Speech.core.so";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#else
#if OS_BUILD
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.os.dll";
        public const CallingConvention NativeCallConvention = CallingConvention.StdCall;
#else
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.dll";
        public const CallingConvention NativeCallConvention = CallingConvention.StdCall;
#endif
#endif
    }

    internal static class SpxExceptionThrower
    {
        internal static void ThrowIfFail(SPXHR hr)
        {
            if (hr != IntPtr.Zero)
            {
                int error = (int)SpxError.GetErrorCode(hr);
                string message = String.Format(CultureInfo.CurrentCulture, "Exception with an error code: 0x{0}", error.ToString("X", CultureInfo.CurrentCulture).ToLower(CultureInfo.CurrentCulture));
                SpxError.Release(hr);
                throw new ApplicationException(message);
            }
        }
    }

    internal static class SpxError
    {
        public static readonly SPXHR BufferTooSmall = (SPXHR)0x019;
        public static readonly SPXHR RuntimeError = (SPXHR)0x01B;
        public static readonly SPXHR InvalidHandle = (SPXHR)0x021;

        public static int GetErrorCode(SPXERRORHANDLE errorHandle)
        {
            int errorCode = (int)error_get_error_code(errorHandle);
            return errorCode;
        }

        public static void Release(SPXERRORHANDLE errorHandle)
        {
            SpxExceptionThrower.ThrowIfFail(error_release(errorHandle));
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        private static extern SPXHR error_get_error_code(SPXERRORHANDLE errorHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        private static extern SPXHR error_release(SPXERRORHANDLE errorHandle);

    }
}
