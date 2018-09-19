//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { CancellationReason, SessionEventArgs } from "./Exports";

/**
 * Defines content of a RecognitionErrorEvent.
 * @class
 */
export class SpeechRecognitionCanceledEventArgs extends SessionEventArgs {
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
