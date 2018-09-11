//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines recognition status.
 */
 export enum RecognitionStatus {
    /**
     * Indicates the result is a phrase that has been successfully recognized.
     */
    Recognized,

    /**
     * Indicates the result is a hypothesis text that has been recognized.
     */
    IntermediateResult,

    /**
     * Indicates that speech was detected in the audio stream, but no words from the target language were matched.
     * Possible reasons could be wrong setting of the target language or wrong format of audio stream.
     */
    NoMatch,

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    InitialSilenceTimeout,

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    InitialBabbleTimeout,

    /**
     * Indicates that an error occurred during recognition.
     */
    Canceled,
}
