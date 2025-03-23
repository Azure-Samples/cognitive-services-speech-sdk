//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.CompilerServices;

namespace Azure.AI.Test.Common.Logging
{
    /// <summary>
    /// A logger implementation that does nothing
    /// </summary>
    public class VoidLogger : ILogger
    {
        private VoidLogger() { }

        /// <summary>
        /// Gets the singleton instance
        /// </summary>
        public static ILogger Instance { get; } = new VoidLogger();

        /// <inheritdoc />
        public string Identifier => string.Empty;

        /// <inheritdoc />
        public LogLevel Level => LogLevel.None;

        /// <inheritdoc />
        public ILogWriter LogWriter => VoidLogWriter.Instance;

        /// <inheritdoc />
        public void Log(LogLevel level, string message, Exception ex = null, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0) {}
    }

    /// <summary>
    /// A log writer that does nothing
    /// </summary>
    public class VoidLogWriter : ILogWriter
    {
        private VoidLogWriter() {}

        /// <summary>
        /// Gets the singleton instance
        /// </summary>
        public static ILogWriter Instance { get; } = new VoidLogWriter();

        /// <inheritdoc />
        public void Dispose() {}

        /// <inheritdoc />
        public void Log(in LogEntry entry) {}
    }
}
