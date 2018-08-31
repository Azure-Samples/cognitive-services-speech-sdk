//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { RecognitionStatus } from "./RecognitionStatus";

/**
 * Defines content of a RecognitionErrorEvent.
 */
export class RecognitionErrorEventArgs {
    /**
     * Specifies the error reason.
     * @return Specifies the error reason.
     */
    public status: RecognitionStatus;

    /**
     * Specifies the session identifier.
     * @return Specifies the session identifier.
     */
    public sessionId: string;
}
