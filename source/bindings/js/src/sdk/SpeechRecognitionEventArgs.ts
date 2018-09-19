//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SessionEventArgs, SpeechRecognitionResult } from "./Exports";

/**
 * Defines contents of speech recognizing/recognized event.
 * @class
 */
export class SpeechRecognitionEventArgs extends SessionEventArgs {
    /**
     * Specifies the recognition result.
     * @property
     * @returns the recognition result.
     */
    public result: SpeechRecognitionResult;
}
