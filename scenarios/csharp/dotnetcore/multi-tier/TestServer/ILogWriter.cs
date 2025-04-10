//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.CompilerServices;
using System.Threading;

namespace Azure.AI.Test.Common.Logging
{
    /// <summary>
    /// Represents an entry to be logged
    /// </summary>
    public readonly ref struct LogEntry
    {
        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="level">The log level of this entry</param>
        /// <param name="message">The message to log</param>
        /// <param name="ex">(Optional) The exception associated with this log entry</param>
        /// <param name="caller">(Optional) The caller that generated this log entry</param>
        /// <param name="file">(Optional) The file that generated this log entry</param>
        /// <param name="line">(Optional) The line number in the source code file that generated this log entry</param>
        public LogEntry(LogLevel level, string message, Exception ex = null, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
        {
            Level = level;
            Message = message;
            Timestamp = DateTimeOffset.UtcNow;
            ThreadId = Thread.CurrentThread.ManagedThreadId;
            Caller = caller;
            File = file;
            Line = line;
            Exception = ex;
        }

        /// <summary>
        /// Gets the log level of this entry
        /// </summary>
        public LogLevel Level { get; }

        /// <summary>
        /// Gets the message to be logged
        /// </summary>
        public string Message { get; }

        /// <summary>
        /// Gets the time stamp of when this log entry was generated
        /// </summary>
        public DateTimeOffset Timestamp { get; }

        /// <summary>
        /// Gets the ID of thread that generated this log entry
        /// </summary>
        public int ThreadId { get; }

        /// <summary>
        /// (Optional) Gets the caller that generated this log entry
        /// </summary>
        public string Caller { get; }

        /// <summary>
        /// (Optional) Gets the source code file that generated this log entry
        /// </summary>
        public string File { get; }

        /// <summary>
        /// (Optional) Gets the line number in the source code file that generated this log entry
        /// </summary>
        public int Line { get; }

        /// <summary>
        /// (Optional) Gets the exception associated with this log entry
        /// </summary>
        public Exception Exception { get; }
    }

    /// <summary>
    /// Base interface for implementations that write log entries
    /// </summary>
    public interface ILogWriter : IDisposable
    {
        /// <summary>
        /// Writes the log entry
        /// </summary>
        /// <param name="entry">The entry to write</param>
        void Log(in LogEntry entry);
    }
}
