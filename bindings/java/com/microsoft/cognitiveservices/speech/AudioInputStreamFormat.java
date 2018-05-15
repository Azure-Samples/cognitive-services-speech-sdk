package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public final class AudioInputStreamFormat {
    /**
     * The format of the audio, valid values: 1 (PCM)
     */
    public short wFormatTag;

    /**
     * The number of channels, valid values: 1 (Mono).
     */
    public short nChannels;

    /**
     * The sample rate, valid values: 16000.
     */
    public int   nSamplesPerSec;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     */
    public int   nAvgBytesPerSec;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     */
    public short nBlockAlign;

    /**
     * The bits per sample, valid values: 16
     */
    public short wBitsPerSample;
}
