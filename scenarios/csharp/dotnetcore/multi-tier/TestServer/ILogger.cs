//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using System.Runtime.CompilerServices;

namespace Azure.AI.Test.Common.Logging
{
    /// <summary>
    /// Enumerates the supported log levels
    /// </summary>
    [Flags]
    public enum LogLevel : uint
    {
        /// <summary>
        /// No logging
        /// </summary>
        None = 0,

        /// <summary>
        /// Showstopping errors have occurred. The program will most likely terminate or exit after this
        /// </summary>
        Critical = 1,

        /// <summary>
        /// Error logs
        /// </summary>
        Error = 2,

        /// <summary>
        /// Warning logs
        /// </summary>
        Warning = 4,

        /// <summary>
        /// Informational logs
        /// </summary>
        Info = 8,

        /// <summary>
        /// Additional logging for debugging. This is usually quite verbose
        /// </summary>
        Trace = 16,

        /// <summary>
        /// Enable all logging
        /// </summary>
        All = 0xffffffff
    }

    /// <summary>
    /// The base interface for a logger
    /// </summary>
    public interface ILogger
    {
        /// <summary>
        /// (Optional) Gets the unique identifier for this logger. This can be used to differentiate between
        /// logs from various sources
        /// </summary>
        string Identifier { get; }

        /// <summary>
        /// Gets the currently enabled log levels
        /// </summary>
        LogLevel Level { get; }

        /// <summary>
        /// Gets the log writer that is responsible for writing the logs
        /// </summary>
        ILogWriter LogWriter { get; }

        /// <summary>
        /// Writes a log entry. This will filter out entries that don't match the current <see cref="Level"/>
        /// flag(s)
        /// </summary>
        /// <param name="level">The level for this log entry</param>
        /// <param name="message">The message to log</param>
        /// <param name="ex">(Optional) The exception associated with this log entry</param>
        /// <param name="caller">(Automatically set) The caller that is creating the log entry</param>
        /// <param name="file">(Automatically set) The full path to the file that is creating this log entry</param>
        /// <param name="line">(Automatically set) The line number in the file that is creating this log entry</param>
        void Log(LogLevel level, string message, Exception ex = null, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0);
    }

    /// <summary>
    /// Helper extension methods for <see cref="ILogger"/> to make it easier to log
    /// </summary>
    public static class ILoggerExtensions
    {
        /// <summary>
        /// Helper method to make working with params log entries easier
        /// </summary>
        /// <param name="args">The params entries to wrap</param>
        /// <returns>The corresponding object array</returns>
        public static object[] Args(params object[] args)
            => args;

        /// <summary>
        /// Writes a log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="level">The level for the log entry</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void Log(this ILogger logger, LogLevel level, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(level, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Writes a critical log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogCritical(this ILogger logger, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Critical, message, null, caller, file, line);

        /// <summary>
        /// Writes a critical log entry with an associated exception
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogCritical(this ILogger logger, Exception ex, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Critical, message, ex, caller, file, line);

        /// <summary>
        /// Writes a critical log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogCritical(this ILogger logger, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Critical, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Writes a critical log entry with an associated exception
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogCritical(this ILogger logger, Exception ex, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Critical, string.Format(CultureInfo.InvariantCulture, format, args), ex, caller, file, line);

        /// <summary>
        /// Writes an error log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogError(this ILogger logger, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Error, message, null, caller, file, line);

        /// <summary>
        /// Writes an error log entry with an associated exception
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogError(this ILogger logger, Exception ex, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Error, message, ex, caller, file, line);

        /// <summary>
        /// Writes an error log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogError(this ILogger logger, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Error, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Writes an error log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogError(this ILogger logger, Exception ex, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Error, string.Format(CultureInfo.InvariantCulture, format, args), ex, caller, file, line);

        /// <summary>
        /// Writes a warning log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogWarning(this ILogger logger, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Warning, message, null, caller, file, line);

        /// <summary>
        /// Writes a warning log entry with an associated exception
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogWarning(this ILogger logger, Exception ex, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Warning, message, ex, caller, file, line);

        /// <summary>
        /// Writes a warning log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogWarning(this ILogger logger, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Warning, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Writes a warning log entry with an associated exception
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="ex">The exception associated with the log entry</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogWarning(this ILogger logger, Exception ex, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Warning, string.Format(CultureInfo.InvariantCulture, format, args), ex, caller, file, line);

        /// <summary>
        /// Writes an informational log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogInfo(this ILogger logger, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Info, message, null, caller, file, line);

        /// <summary>
        /// Writes an informational log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogInfo(this ILogger logger, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Info, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Writes a trace log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="message">The message to include in the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogTrace(this ILogger logger, string message, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Trace, message, null, caller, file, line);

        /// <summary>
        /// Writes a trace log entry
        /// </summary>
        /// <param name="logger">The logger instance to log to</param>
        /// <param name="format">The format string used to generate the log entry</param>
        /// <param name="args">The arguments used to generate the log entry</param>
        /// <param name="caller">(Auto set) The caller that is creating the log entry</param>
        /// <param name="file">(Auto set) The full path to the file that is creating the log entry</param>
        /// <param name="line">(Auto set) The line number in the file that is creating the log entry</param>
        public static void LogTrace(this ILogger logger, string format, object[] args, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
            => logger?.Log(LogLevel.Trace, string.Format(CultureInfo.InvariantCulture, format, args), null, caller, file, line);

        /// <summary>
        /// Gets the string representation of the log level
        /// </summary>
        /// <param name="level">The log level</param>
        /// <returns>The corresponding string representation</returns>
        public static string ToString(LogLevel level)
        {
            switch (level)
            {
                default: return "-";
                case LogLevel.Critical: return "Critical";
                case LogLevel.Error: return "Error";
                case LogLevel.Warning: return "Warning";
                case LogLevel.Info: return "Info";
                case LogLevel.Trace: return "Trace";
            }
        }
    }
}
