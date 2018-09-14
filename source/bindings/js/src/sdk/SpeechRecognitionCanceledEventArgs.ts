//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { CancellationReason, SpeechRecognitionResult } from "./Exports";

/**
 * Defines content of a RecognitionErrorEvent.
 * @class
 */
export class SpeechRecognitionCanceledEventArgs {
    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public result: SpeechRecognitionResult;

    /**
     * Specifies the session identifier.
     * @property
     * @returns Specifies the session identifier.
     */
    public sessionId: string;

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public reason: CancellationReason;

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @return A String that represents the error details.
     */
    public errorDetails: string;
}
