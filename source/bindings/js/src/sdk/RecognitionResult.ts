//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { PropertyCollection, ResultReason } from "./Exports";

/**
 * Defines result of speech recognition.
 * @class
 */
export class RecognitionResult {
    /**
     * Specifies the result identifier.
     * @property
     * @returns Specifies the result identifier.
     */
    public resultId: string;

    /**
     * Specifies status of the result.
     * @property
     * @returns Specifies status of the result.
     */
    public reason: ResultReason;

    /**
     * Presents the recognized text in the result.
     * @property
     * @returns Presents the recognized text in the result.
     */
    public text: string;

    /**
     * Duration of recognized speech in milliseconds.
     * @property
     * @returns Duration of recognized speech in milliseconds.
     */
    public duration: number;

    /**
     * Offset of recognized speech in milliseconds.
     * @property
     * @returns Offset of recognized speech in milliseconds.
     */
    public offset: number;

    /**
     * In case of an unsuccessful recognition, provides a brief description of an occurred error.
     * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
     * @property
     * @returns a brief description of an error.
     */
    public errorDetails: string;

    /**
     * A string containing Json serialized recognition result as it was received from the service.
     * @property
     * @returns Json serialized representation of the result.
     */
    public json: string;

    /**
     *  The set of properties exposed in the result.
     * @property
     * @returns The set of properties exposed in the result.
     */
    public properties: PropertyCollection;
}
