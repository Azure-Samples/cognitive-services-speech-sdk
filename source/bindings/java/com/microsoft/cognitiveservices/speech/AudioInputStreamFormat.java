package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public final class AudioInputStreamFormat {
    /**
     * The format of the audio, valid values: 1 (PCM)
     */
    public int FormatTag;

    /**
     * The number of channels, valid values: 1 (Mono).
     */
    public int Channels;

    /**
     * The sample rate, valid values: 16000.
     */
    public int   SamplesPerSec;

    /**
     * Average bytes per second, usually calculated as nSamplesPerSec * nChannels * ceil(wBitsPerSample, 8).
     */
    public int   AvgBytesPerSec;

    /**
     * The size of a single frame, valid values: nChannels * ceil(wBitsPerSample, 8).
     */
    public int BlockAlign;

    /**
     * The bits per sample, valid values: 16
     */
    public int BitsPerSample;
}
