//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Defines the possible reasons a recognition result might be canceled.
 */
export enum CancellationReason {
    /**
     * Indicates that an error occurred during speech recognition. Use getErrorDetails() contains detailed error response.
     */
    Error,

    /**
     * Indicates that the end of the audio stream was reached.
     */
    EndOfStream,
}
