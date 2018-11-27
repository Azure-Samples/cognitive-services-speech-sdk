// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

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
