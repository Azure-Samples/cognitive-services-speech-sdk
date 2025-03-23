//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.CompilerServices;

namespace Azure.AI.Test.Common.Logging
{
    public class SystemDebugLogger : ILogger
    {
        private SystemDebugLogger() { }
        public static ILogger Instance { get; } = new SystemDebugLogger();
        public string Identifier => string.Empty;
        public LogLevel Level => LogLevel.All;
        public ILogWriter LogWriter => SystemDebugLogWriter.Instance;
        public void Log(LogLevel level, string message, Exception ex = null, [CallerMemberName] string caller = null, [CallerFilePath] string file = null, [CallerLineNumber] int line = 0)
        {
            System.Diagnostics.Debug.WriteLine($"{level} {caller} {file} {line} {message} {ex}");
        }
    }

    public class SystemDebugLogWriter : ILogWriter
    {
        private SystemDebugLogWriter() { }
        public static ILogWriter Instance { get; } = new SystemDebugLogWriter();
        public void Dispose() { }
        public void Log(in LogEntry entry)
        {
            System.Diagnostics.Debug.WriteLine($"{entry.Level} {entry.Caller} {entry.File} {entry.Line} {entry.Message} {entry.Exception}");
        }
    }
}
