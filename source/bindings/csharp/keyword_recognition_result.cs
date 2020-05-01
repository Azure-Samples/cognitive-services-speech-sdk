//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Class that defines the results emitted by the <see cref="KeywordRecognizer" />.
    /// </summary>
    public class KeywordRecognitionResult : RecognitionResult
    {
        internal KeywordRecognitionResult(IntPtr resultPtr) : base(resultPtr)
        {
        }

    }
}
