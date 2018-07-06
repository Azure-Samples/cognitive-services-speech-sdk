//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    class AssertHelpers
    {
        static public void AssertStringContains(string inputString, string searchString)
        {
            Assert.IsTrue(inputString.Contains(searchString),
                $"Input string does not contain '{searchString}': '{inputString}'");
        }
    }
}
