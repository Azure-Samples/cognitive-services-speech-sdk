// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

/**
 * @class SynthesisStatus
 * @private
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

export enum RecognitionStatus {
    Success,
    NoMatch,
    InitialSilenceTimeout,
    BabbleTimeout,
    Error,
    EndOfDictation,
}
