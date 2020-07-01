//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;

    /// <summary>
    /// Methods to start and stop the file logger
    /// </summary>
    internal static class FileLogger
    {
        private static readonly InteropSafeHandle ROOT_SITE_PROPERTY_BAG_HANDLE =
            new InteropSafeHandle(new IntPtr(1), _ => IntPtr.Zero); // don't destroy when it goes out of scope

        /// <summary>
        /// Starts logging to a file.
        /// </summary>
        /// <param name="logFile">The file to log to.</param>
        /// <returns>The status code indicating success or the failure cause.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static SPXHR StartLogging(string logFile)
        {
            if (string.IsNullOrWhiteSpace(logFile))
            {
                throw new ArgumentException(nameof(logFile) + " cannot be null, empty or consist only of white space");
            }

            IntPtr nativeUtf8String = IntPtr.Zero;
            try
            {
                nativeUtf8String = Utf8StringMarshaler.MarshalManagedToNative(logFile);

                return RunCommands(
                    () => PropertyCollection.property_bag_set_string(
                            ROOT_SITE_PROPERTY_BAG_HANDLE,
                            (int)PropertyId.Speech_LogFilename,
                            IntPtr.Zero,
                            nativeUtf8String),
                    () => diagnostics_log_start_logging(ROOT_SITE_PROPERTY_BAG_HANDLE, IntPtr.Zero));
            }
            finally
            {
                if (nativeUtf8String != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(nativeUtf8String);
                }
            }
        }

        /// <summary>
        /// Stops logging to a file. This releases the file so it can be accessed by other processes.
        /// </summary>
        /// <returns>The status code indicating success or the failure cause.</returns>
        public static SPXHR StopLogging() => diagnostics_log_stop_logging();

        /// <summary>
        /// Resets all logging parameters set
        /// </summary>
        /// <returns>The status code indicating success or the failure cause.</returns>
        public static SPXHR ResetLogging()
        {
            IntPtr nativeUtf8String = IntPtr.Zero;
            try
            {
                nativeUtf8String = Utf8StringMarshaler.MarshalManagedToNative(string.Empty);

                return RunCommands(
                    () => diagnostics_log_stop_logging(),
                    () => PropertyCollection.property_bag_set_string(
                            ROOT_SITE_PROPERTY_BAG_HANDLE,
                            (int)PropertyId.Speech_LogFilename,
                            IntPtr.Zero,
                            nativeUtf8String),
                    () => diagnostics_log_apply_properties(ROOT_SITE_PROPERTY_BAG_HANDLE, IntPtr.Zero));
            }
            finally
            {
                if (nativeUtf8String != IntPtr.Zero)
                {
                    Marshal.FreeHGlobal(nativeUtf8String);
                }
            }
        }

        private static SPXHR RunCommands(params Func<SPXHR>[] commands)
        {
            SPXHR result = IntPtr.Zero;

            foreach (var cmd in commands)
            {
                result = cmd();
                if (result != IntPtr.Zero)
                {
                    return result;
                }
            }

            return result;
        }

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        private static extern SPXHR diagnostics_log_start_logging(InteropSafeHandle speechconfig, IntPtr reserved);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        private static extern SPXHR diagnostics_log_apply_properties(InteropSafeHandle speechconfig, IntPtr reserved);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        private static extern SPXHR diagnostics_log_stop_logging();
    }
}
