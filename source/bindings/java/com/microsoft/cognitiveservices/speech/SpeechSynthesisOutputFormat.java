//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the possible speech synthesis output audio format.
 * Added in version 1.7.0
 */
public enum SpeechSynthesisOutputFormat
{
    /**
     *  raw-8khz-8bit-mono-mulaw
     */
    Raw8Khz8BitMonoMULaw,

    /**
     *  riff-16khz-16kbps-mono-siren
     */
    Riff16Khz16KbpsMonoSiren,

    /**
     *  audio-16khz-16kbps-mono-siren
     */
    Audio16Khz16KbpsMonoSiren,

    /**
     *  audio-16khz-32kbitrate-mono-mp3
     */
    Audio16Khz32KBitRateMonoMp3,

    /**
     *  audio-16khz-128kbitrate-mono-mp3
     */
    Audio16Khz128KBitRateMonoMp3,

    /**
     *  audio-16khz-64kbitrate-mono-mp3
     */
    Audio16Khz64KBitRateMonoMp3,

    /**
     *  audio-24khz-48kbitrate-mono-mp3
     */
    Audio24Khz48KBitRateMonoMp3,

    /**
     *  audio-24khz-96kbitrate-mono-mp3
     */
    Audio24Khz96KBitRateMonoMp3,

    /**
     *  audio-24khz-160kbitrate-mono-mp3
     */
    Audio24Khz160KBitRateMonoMp3,

    /**
     *  raw-16khz-16bit-mono-truesilk
     */
    Raw16Khz16BitMonoTrueSilk,

    /**
     *  riff-16khz-16bit-mono-pcm
     */
    Riff16Khz16BitMonoPcm,

    /**
     *  riff-8khz-16bit-mono-pcm
     */
    Riff8Khz16BitMonoPcm,

    /**
     *  riff-24khz-16bit-mono-pcm
     */
    Riff24Khz16BitMonoPcm,

    /**
     *  riff-8khz-8bit-mono-mulaw
     */
    Riff8Khz8BitMonoMULaw,

    /**
     *  raw-16khz-16bit-mono-pcm
     */
    Raw16Khz16BitMonoPcm,

    /**
     *  raw-24khz-16bit-mono-pcm
     */
    Raw24Khz16BitMonoPcm,

    /**
     *  raw-8khz-16bit-mono-pcm
     */
    Raw8Khz16BitMonoPcm,

    /**
     * ogg-16khz-16bit-mono-opus
     */
    Ogg16Khz16BitMonoOpus,

    /**
     * ogg-24khz-16bit-mono-opus
     */
    Ogg24Khz16BitMonoOpus
}
