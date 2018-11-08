//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines the possible reasons a recognition result might be canceled.
 * @class CancellationReason
 */
export enum CancellationReason {
    /**
     * Indicates that an error occurred during speech recognition.
     * @member CancellationReason.Error
     */
    Error,

    /**
     * Indicates that the end of the audio stream was reached.
     * @member CancellationReason.EndOfStream
     */
    EndOfStream,
}
