//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.CompilerServices;

namespace Azure.AI.Test.Common.Logging
{
    /// <summary>
    /// A simple implementation of a logger
    /// </summary>
    public class Logger : ILogger
    {
        /// <summary>
        /// Creates a new instance. This will filter out <see cref="LogLevel.Trace"/> logs
        /// </summary>
        /// <param name="writer">Where to write the log entries to</param>
        /// <param name="id">(Optional) An identifier for the logs</param>
        public Logger(ILogWriter writer, string id = null)
            : this(writer, LogLevel.Critical | LogLevel.Error | LogLevel.Warning | LogLevel.Info, id)
        {
        }

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="writer">Where to write the log entries to</param>
        /// <param name="logLevels">The enabled log levels</param>
        /// <param name="id">(Optional) An identifier for the logs</param>
        public Logger(ILogWriter writer, LogLevel logLevels, string id = null)
        {
            Identifier = id;
            Level = logLevels;
            LogWriter = writer ?? throw new ArgumentNullException(nameof(writer));
        }

        /// <inheritdoc />
        public string Identifier { get; internal set; }

        /// <inheritdoc />
        public LogLevel Level { get; internal set; }

        /// <inheritdoc />
        public ILogWriter LogWriter { get; internal set; }

        /// <inheritdoc />
        public void Log(LogLevel level, string message, Exception ex = null, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
        {
            if (!Level.HasFlag(level))
            {
                return;
            }

            string logMessage = message;
            if (Identifier != null)
            {
                logMessage = Identifier + " " + message;
            }

            LogWriter?.Log(new LogEntry(level, logMessage, ex, caller, file, line));
        }
    }
}
