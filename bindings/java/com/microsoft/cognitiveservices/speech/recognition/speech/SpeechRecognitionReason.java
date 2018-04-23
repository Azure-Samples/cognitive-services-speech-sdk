package com.microsoft.cognitiveservices.speech.recognition.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines speech recognition status.
  */
public enum SpeechRecognitionReason
{
    Recognized,
    IntermediateResult,
    NoMatch,
    Canceled,
    OtherRecognizer
}
