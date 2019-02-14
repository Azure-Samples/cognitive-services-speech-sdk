//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    public static class MemoryLeakDetector
    {
        /// <summary>
        /// Runs a specified async method and looks for memory increase in the current processes footprint.
        /// </summary>
        /// <param name="func">Async function to call. Multiple calls are run synchronously.</param>
        /// <param name="loopsToStablize">How many times the method should be run to get a baseline. Default = 5.</param>
        /// <param name="loopsToTest">Number of loops to run before checking for memory increase. Default=50</param>
        /// <param name="allowedVariance">The variance (as a multiplier) from the baseline to allow. Default is up to 110% of the baseline is considered success.</param>
        public static bool TestForLeaks(Func<Task> func,
                                        int loopsToStablize = 5,
                                        int loopsToTest = 500,
                                        double allowedVariance = 1.1)
        {
            for (int i = 0; i < loopsToStablize; i++)
            {
                func().Wait();
            }

            GC.Collect(GC.MaxGeneration);
            GC.WaitForPendingFinalizers();
            long memoryBaseline = GC.GetTotalMemory(true); ;
            for (int i = 0; i < loopsToTest; i++)
            {
                func().Wait();
            }

            GC.Collect(GC.MaxGeneration);
            GC.WaitForPendingFinalizers();
            long after = GC.GetTotalMemory(true);
            if (after > (memoryBaseline * allowedVariance))
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
}
