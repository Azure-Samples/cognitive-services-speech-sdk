//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    public class RetryTestMethod : DataTestMethodAttribute
    {
        private int retryCount = 0;
        private bool retryNeeded = false;
        private const int MaxRetryCount = 3;
        private TestResult[] testResult = null;
        public override TestResult[] Execute(ITestMethod testMethod)
        {
            do
            {
                testResult = Invoke(testMethod);
                retryNeeded = TestFailed(testMethod.TestMethodName);
                if (retryNeeded)
                {
                    retryCount++;
                }
            }
            while (retryNeeded && retryCount < MaxRetryCount);
            retryCount = 0;
            return testResult;
        }

        private TestResult[] Invoke(ITestMethod testMethod)
        {
            return new[] { testMethod.Invoke(null) };
        }

        private bool TestFailed(string testMethodName)
        {
            bool testFailed = false;
            foreach (var res in testResult)
            {
                if (res.Outcome == UnitTestOutcome.Failed)
                {
                    Console.WriteLine($"Test method: {testMethodName} run: {retryCount} failed");
                    testFailed = true;
                }
            }
            return testFailed;
        }
    }
}
