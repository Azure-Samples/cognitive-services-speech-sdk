//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import {
    CancellationReason,
    TranslationTextResult,
} from "./Exports";
/**
 * Define payload of speech recognition canceled result events.
 */
export class TranslationTextResultCanceledEventArgs {

    private privResult: TranslationTextResult;
    private privSessionId: string;
    private privCancelReason: CancellationReason;
    private privErrorDetails: string;

    public constructor(sessionid: string, cancellationReason: CancellationReason, errorDetails: string, result: TranslationTextResult) {
        this.privCancelReason = cancellationReason;
        this.privErrorDetails = errorDetails;
        this.privResult = result;
        this.privSessionId = sessionid;
    }

    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public get result(): TranslationTextResult {
        return this.privResult;
    }

    /**
     * Specifies the session identifier.
     * @return the session identifier.
     */
    public get sessionId(): string {
        return this.privSessionId;
    }

    /**
     * The reason the recognition was canceled.
     * @return Specifies the reason canceled.
     */
    public get reason(): CancellationReason {
        return this.privCancelReason;
    }

    /**
     * In case of an unsuccessful recognition, provides a details of why the occurred error.
     * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
     * @return A String that represents the error details.
     */
    public get errorDetails(): string {
        return this.privErrorDetails;
    }
}
