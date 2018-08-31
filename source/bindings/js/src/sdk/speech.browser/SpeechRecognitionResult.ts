//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { ISpeechProperties, RecognitionStatus } from "./Exports";

/**
 * Defines result of speech recognition.
 */
export class SpeechRecognitionResult {
    /**
     * Specifies the result identifier.
     * @return Specifies the result identifier.
     */
    public resultId: string;

    /**
     * Specifies status of the result.
     * @return Specifies status of the result.
     */
    public reason: RecognitionStatus;

    /**
     * Presents the recognized text in the result.
     * @return Presents the recognized text in the result.
     */
    public text: string;

    /**
     * Duration of recognized speech in milliseconds.
     * @return Duration of recognized speech in milliseconds.
     */
    public duration: number;

    /**
     * Offset of recognized speech in milliseconds.
     * @return Offset of recognized speech in milliseconds.
     */
    public offset: number;

    /**
     * In case of an unsuccessful recognition, provides a brief description of an occurred error.
     * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
     * @return a brief description of an error.
     */
    public errorDetails: string;

    /**
     * A string containing Json serialized recognition result as it was received from the service.
     * @return Json serialized representation of the result.
     */
    public json: string;

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public properties: ISpeechProperties;
}
