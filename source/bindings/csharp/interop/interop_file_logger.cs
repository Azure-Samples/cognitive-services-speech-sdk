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
        /// <summary>
        /// Starts logging to a file.
        /// </summary>
        /// <param name="logFile">The file to log to.</param>
        /// <param name="appendToFile">Set to true to append to the file if it already exists. False will overwrite existing files.</param>
        /// <param name="filters">The filters that control what gets logged to the file.</param>
        /// <param name="rolloverSizeInMegaBytes">The maximum size of the log file before a new one is created. Set to 0 to log to a single file.</param>
        /// <param name="rolloverDuration">How many seconds of logs each file can contain. Must in be in whole unit seconds.</param>
        /// <returns>The status code indicating success or the failure cause.</returns>
        public static SPXHR StartLogging(string logFile, bool appendToFile = false, string filters = null, uint rolloverSizeInMegaBytes = 0, TimeSpan? rolloverDuration = null)
        {
            if (string.IsNullOrWhiteSpace(logFile))
            {
                throw new ArgumentException(nameof(logFile) + " cannot be null, empty or consist only of white space");
            }
            else if (rolloverDuration != null)
            {
                if (rolloverDuration.Value.TotalSeconds < 1)
                {
                    throw new ArgumentOutOfRangeException(nameof(rolloverDuration), "Rollover duration must be at least 1 second");
                }
                else if (Math.Abs(rolloverDuration.Value.TotalSeconds - (int)rolloverDuration.Value.TotalSeconds) > 0.001)
                {
                    throw new ArgumentOutOfRangeException(nameof(rolloverDuration), "The rollover duration must be in whole second units");
                }
            }

            // we create a dummy speech config to wrap the file logger properties we want to set
            var speechConfig = Speech.SpeechConfig.FromSubscription("not_real", "not_real");

            speechConfig.SetProperty(Speech.PropertyId.Speech_LogFilename, logFile);
            if (appendToFile)
            {
                speechConfig.SetProperty("SPEECH-AppendToLogFile", "1");
            }

            if (!string.IsNullOrWhiteSpace(filters))
            {
                speechConfig.SetProperty("SPEECH-FileLogFilters", filters);
            }

            if (rolloverSizeInMegaBytes > 0)
            {
                speechConfig.SetProperty("SPEECH-FileLogSizeMB", rolloverSizeInMegaBytes.ToString(CultureInfo.InvariantCulture));
            }

            if (rolloverDuration.HasValue)
            {
                speechConfig.SetProperty("SPEECH-FileLogDurationSeconds", ((int)rolloverDuration.Value.TotalSeconds).ToString(CultureInfo.InvariantCulture));
            }

            return diagnostics_log_apply_properties(speechConfig.configHandle, (IntPtr)0);
        }

        /// <summary>
        /// Stops logging to a file. This releases the file so it can be accessed by other processes.
        /// </summary>
        /// <returns>The status code indicating success or the failure cause.</returns>
        public static SPXHR StopLogging() => diagnostics_log_stop_logging();

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        private static extern SPXHR diagnostics_log_apply_properties(InteropSafeHandle speechconfig, IntPtr reserved);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        private static extern SPXHR diagnostics_log_stop_logging();
    }
}
