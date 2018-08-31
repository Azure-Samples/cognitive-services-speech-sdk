//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
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
