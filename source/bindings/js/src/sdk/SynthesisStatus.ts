//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export enum SynthesisStatus {
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
    Error,
}
