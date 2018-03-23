//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon.Recognition.Speech
{
    /// <summary>
    /// Defines speech recognition status.
    /// </summary>
    public enum SpeechRecognitionReason
    {
        Recognized,
        IntermediateResult,
        NoMatch,
        Canceled,
        OtherRecognizer
    }
}
