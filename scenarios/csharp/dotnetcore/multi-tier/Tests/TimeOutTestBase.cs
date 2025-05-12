//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace SpeechSDKSamples.Remote.Service.Tests
{
    public class TimeOutTestBase
    {
        private readonly static TimeSpan _actionTimeout = Debugger.IsAttached ? TimeSpan.FromMinutes(15) : TimeSpan.FromSeconds(15);
        private readonly static TimeSpan _testTimeout = Debugger.IsAttached ? TimeSpan.FromMinutes(60) : TimeSpan.FromSeconds(60);

        private CancellationToken _timeoutToken = new CancellationTokenSource(_testTimeout).Token;

        protected async Task TimeoutTestAction(Task action, string message = "")
        {
            var t = await Task.WhenAny(action, Task.Delay(_actionTimeout, _timeoutToken)).ConfigureAwait(false);
            Assert.AreEqual(action, t, message);
        }

        protected Task TimeoutTestAction<T>(TaskCompletionSource<T> action, string message = "") =>
            TimeoutTestAction(action.Task, message);

        protected Task TimeoutTestAction(TaskCompletionSource action, string message = "") =>
            TimeoutTestAction(action.Task, message);

        protected static TimeSpan ActionTimeout { get => _actionTimeout; }
        protected static TimeSpan TestTimeout { get => _testTimeout; }
        protected CancellationToken TimeoutToken { get => _timeoutToken; }
    }
}
