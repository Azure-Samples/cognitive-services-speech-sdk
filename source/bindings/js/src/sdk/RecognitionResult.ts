//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
import { PropertyCollection, ResultReason } from "./Exports";

/**
 * Defines result of speech recognition.
 * @class
 */
export class RecognitionResult {
    private privResultId: string;
    private privReason: ResultReason;
    private privText: string;
    private privDuration: number;
    private privOffset: number;
    private privErrorDetails: string;
    private privJson: string;
    private privProperties: PropertyCollection;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param resultId The result id.
     * @param reason The reason.
     * @param text The recognized text.
     * @param duration The duration.
     * @param offset The offset into the stream.
     * @param errorDetails Error details, if provided.
     * @param json Additional Json, if provided.
     * @param properties Additional properties, if provided.
     */
    constructor(resultId?: string, reason?: ResultReason, text?: string, duration?: number, offset?: number, errorDetails?: string, json?: string, properties?: PropertyCollection) {
        this.privResultId = resultId;
        this.privReason = reason;
        this.privText = text;
        this.privDuration = duration;
        this.privOffset = offset;
        this.privErrorDetails = errorDetails;
        this.privJson = json;
        this.privProperties = properties;
    }

    /**
     * Specifies the result identifier.
     * @property
     * @returns Specifies the result identifier.
     */
    public get resultId(): string {
        return this.privResultId;
    }

    /**
     * Specifies status of the result.
     * @property
     * @returns Specifies status of the result.
     */
    public get reason(): ResultReason {
        return this.privReason;
    }

    /**
     * Presents the recognized text in the result.
     * @property
     * @returns Presents the recognized text in the result.
     */
    public get text(): string {
        return this.privText;
    }

    /**
     * Duration of recognized speech in milliseconds.
     * @property
     * @returns Duration of recognized speech in milliseconds.
     */
    public get duration(): number {
        return this.privDuration;
    }

    /**
     * Offset of recognized speech in milliseconds.
     * @property
     * @returns Offset of recognized speech in milliseconds.
     */
    public get offset(): number {
        return this.privOffset;
    }

    /**
     * In case of an unsuccessful recognition, provides a brief description of an occurred error.
     * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
     * @property
     * @returns a brief description of an error.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }

    /**
     * A string containing Json serialized recognition result as it was received from the service.
     * @property
     * @returns Json serialized representation of the result.
     */
    public get json(): string {
        return this.privJson;
    }

    /**
     *  The set of properties exposed in the result.
     * @property
     * @returns The set of properties exposed in the result.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }
}
