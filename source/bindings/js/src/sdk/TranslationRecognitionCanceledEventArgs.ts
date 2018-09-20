//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import {
    CancellationReason,
    TranslationRecognitionResult,
} from "./Exports";
/**
 * Define payload of speech recognition canceled result events.
 * @class TranslationRecognitionCanceledEventArgs
 */
export class TranslationRecognitionCanceledEventArgs {
    private privResult: TranslationRecognitionResult;
    private privSessionId: string;
    private privCancelReason: CancellationReason;
    private privErrorDetails: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param sessionid - The session id.
     * @param cancellationReason - The cancellation reason.
     * @param errorDetails - Error details, if provided.
     * @param result - The result.
     */
    public constructor(sessionid: string, cancellationReason: CancellationReason, errorDetails: string, result: TranslationRecognitionResult) {
        this.privCancelReason = cancellationReason;
        this.privErrorDetails = errorDetails;
        this.privResult = result;
        this.privSessionId = sessionid;
    }

    /**
     * Specifies the recognition result.
     * @member TranslationRecognitionCanceledEventArgs.prototype.result
     * @return the recognition result.
     */
    public get result(): TranslationRecognitionResult {
        return this.privResult;
    }

    /**
     * Specifies the session identifier.
     * @member TranslationRecognitionCanceledEventArgs.prototype.sessionId
     * @return the session identifier.
     */
    public get sessionId(): string {
        return this.privSessionId;
    }

    /**
     * The reason the recognition was canceled.
     * @member TranslationRecognitionCanceledEventArgs.prototype.reason
     * @return Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privCancelReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @member TranslationRecognitionCanceledEventArgs.prototype.errorDetails
     * @return A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
