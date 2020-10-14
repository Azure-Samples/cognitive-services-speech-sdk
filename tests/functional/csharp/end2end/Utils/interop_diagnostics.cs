//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Test.Internal
{
    using SPXHR = System.IntPtr;
    using SPXERRORHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void LogMessageCallbackFunctionDelegate([MarshalAs(UnmanagedType.LPStr)] string logMessage);

    internal class Diagnostics
    {
        // Limit the length by 4 MB for safety check if something bad has happened on the native side.
        private const int lengthLimit = 1 << 22;

        public const int __SPX_TRACE_LEVEL_INFO = 0x08; // Trace_Info
        public const int __SPX_TRACE_LEVEL_WARNING = 0x04; // Trace_Warning
        public const int __SPX_TRACE_LEVEL_ERROR = 0x02; // Trace_Error
        public const int __SPX_TRACE_LEVEL_VERBOSE = 0x10; // Trace_Verbose

        public static string ReadNativeUtf8String(IntPtr native)
        {
            // Copied from source/bindings/csharp/interop/interop_utf8_string_marshaler.cs
            if (native == IntPtr.Zero)
            {
                return string.Empty;
            }

            // Identifying the length of the UTF8 string by searching for 0 byte.
            int lengthInBytes = 0;
            while (Marshal.ReadByte(native + lengthInBytes) != 0 && lengthInBytes < lengthLimit)
            {
                lengthInBytes++;
            }

            if (lengthInBytes >= lengthLimit)
            {
                throw new ArgumentException($"Provided data is not a string or it has the size exceeding ${lengthLimit} bytes.");
            }

            var buffer = new byte[lengthInBytes];

            // Performance wise this can be improved by switching to unsafe code and avoiding copying.
            Marshal.Copy(native, buffer, 0, lengthInBytes);
            return Encoding.UTF8.GetString(buffer);
        }

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern void diagnostics_log_trace_string(
            int level, [MarshalAs(UnmanagedType.LPStr)] string title,
            [MarshalAs(UnmanagedType.LPStr)] string fileName, int lineNumber,
            [MarshalAs(UnmanagedType.LPStr)] string message);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR diagnostics_logmessage_set_callback(LogMessageCallbackFunctionDelegate callback);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR diagnostics_logmessage_set_filters([MarshalAs(UnmanagedType.LPStr)] string filters);


        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern void diagnostics_log_memory_start_logging();

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern void diagnostics_log_memory_stop_logging();


        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern int diagnostics_log_memory_get_line_num_oldest();

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern int diagnostics_log_memory_get_line_num_newest();

        public static string diagnostics_log_memory_get_line(int lineNum)
        {
            /*
             * NOTES:
             * - [MarshalAs(UnmanagedType.LPStr)] incorrectly tries to free the native memory resulting in exceptions
             * - Marshal.PtrToStringAnsi mangles all UTF-8 character sequences beyond the standard ANSI range
             * - Native code returns UTF-8 so we can't use Marshal.PtrToStringUni
             */
            IntPtr nativeString = diagnostics_log_memory_get_line_internal(lineNum);
            return ReadNativeUtf8String(nativeString);
        }

        [DllImport(Import.NativeDllName, EntryPoint = "diagnostics_log_memory_get_line", CallingConvention = Import.NativeCallConvention)]
        private static extern IntPtr diagnostics_log_memory_get_line_internal(int lineNum);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern void diagnostics_log_memory_dump(
            [MarshalAs(UnmanagedType.LPStr)] string filename,
            [MarshalAs(UnmanagedType.LPStr)] string linePrefix,
            bool emitToStdOut,
            bool emitToStdErr);
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
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.dll";
        public const CallingConvention NativeCallConvention = CallingConvention.StdCall;
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
