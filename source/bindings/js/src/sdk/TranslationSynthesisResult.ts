// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ResultReason } from "./ResultReason";

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 * @class TranslationSynthesisResult
 */
export class TranslationSynthesisResult {
    private privReason: ResultReason;
    private privAudio: ArrayBuffer;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {ResultReason} reason - The synthesis reason.
     * @param {ArrayBuffer} audio - The audio data.
     */
    constructor(reason: ResultReason, audio: ArrayBuffer) {
        this.privReason = reason;
        this.privAudio = audio;
    }

    /**
     * Translated text in the target language.
     * @member TranslationSynthesisResult.prototype.audio
     * @function
     * @public
     * @returns {ArrayBuffer} Translated audio in the target language.
     */
    public get audio(): ArrayBuffer {
        return this.privAudio;
    }

    /**
     * The synthesis status.
     * @member TranslationSynthesisResult.prototype.reason
     * @function
     * @public
     * @returns {ResultReason} The synthesis status.
     */
    public get reason(): ResultReason {
        return this.privReason;
    }
}
