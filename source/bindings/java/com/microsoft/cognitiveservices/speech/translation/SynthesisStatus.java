package com.microsoft.cognitiveservices.speech.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
  * Defines synthesis status.
  */
public enum SynthesisStatus
{
    /**
     * The response contains valid audio data.
     */
  Success,
    /**
     * Indicates the end of audio data. No valid audio data is included in the message.
     */
  SynthesisEnd,
    /**
     * Indicates an error occurred during synthesis data processing.
     */
  Error
}
