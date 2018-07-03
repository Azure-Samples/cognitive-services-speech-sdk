package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines recognition status.
  */
public enum RecognitionStatus
{
    Recognized,
    IntermediateResult,
    NoMatch,
    InitialSilenceTimeout,
    InitialBabbleTimeout,
    Canceled
}

