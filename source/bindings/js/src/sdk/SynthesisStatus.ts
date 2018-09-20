//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * @class SynthesisStatus
 */
export enum SynthesisStatus {
    /**
     * The response contains valid audio data.
     * @member SynthesisStatus.Success
     */
    Success,

    /**
     * Indicates the end of audio data. No valid audio data is included in the message.
     * @member SynthesisStatus.SynthesisEnd
     */
    SynthesisEnd,

    /**
     * Indicates an error occurred during synthesis data processing.
     * @member SynthesisStatus.Error
     */
    Error,
}
