//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines the possible speech synthesis output audio format.
    /// Added in version 1.4.0
    /// </summary>
    public enum SpeechSynthesisOutputFormat
    {
        /// <summary>
        /// raw-8khz-8bit-mono-mulaw
        /// </summary>
        Raw8Khz8BitMonoMULaw = 1,

        /// <summary>
        /// riff-16khz-16kbps-mono-siren
        /// </summary>
        Riff16Khz16KbpsMonoSiren = 2,

        /// <summary>
        /// audio-16khz-16kbps-mono-siren
        /// </summary>
        Audio16Khz16KbpsMonoSiren = 3,

        /// <summary>
        /// audio-16khz-32kbitrate-mono-mp3
        /// </summary>
        Audio16Khz32KBitRateMonoMp3 = 4,

        /// <summary>
        /// audio-16khz-128kbitrate-mono-mp3
        /// </summary>
        Audio16Khz128KBitRateMonoMp3 = 5,

        /// <summary>
        /// audio-16khz-64kbitrate-mono-mp3
        /// </summary>
        Audio16Khz64KBitRateMonoMp3 = 6,

        /// <summary>
        /// audio-24khz-48kbitrate-mono-mp3
        /// </summary>
        Audio24Khz48KBitRateMonoMp3 = 7,

        /// <summary>
        /// audio-24khz-96kbitrate-mono-mp3
        /// </summary>
        Audio24Khz96KBitRateMonoMp3 = 8,

        /// <summary>
        /// audio-24khz-160kbitrate-mono-mp3
        /// </summary>
        Audio24Khz160KBitRateMonoMp3 = 9,

        /// <summary>
        /// raw-16khz-16bit-mono-truesilk
        /// </summary>
        Raw16Khz16BitMonoTrueSilk = 10,

        /// <summary>
        /// riff-16khz-16bit-mono-pcm
        /// </summary>
        Riff16Khz16BitMonoPcm = 11,

        /// <summary>
        /// riff-8khz-16bit-mono-pcm
        /// </summary>
        Riff8Khz16BitMonoPcm = 12,

        /// <summary>
        /// riff-24khz-16bit-mono-pcm
        /// </summary>
        Riff24Khz16BitMonoPcm = 13,

        /// <summary>
        /// riff-8khz-8bit-mono-mulaw
        /// </summary>
        Riff8Khz8BitMonoMULaw = 14,

        /// <summary>
        /// raw-16khz-16bit-mono-pcm
        /// </summary>
        Raw16Khz16BitMonoPcm = 15,

        /// <summary>
        /// raw-24khz-16bit-mono-pcm
        /// </summary>
        Raw24Khz16BitMonoPcm = 16,

        /// <summary>
        /// raw-8khz-16bit-mono-pcm
        /// </summary>
        Raw8Khz16BitMonoPcm = 17
    }
}
