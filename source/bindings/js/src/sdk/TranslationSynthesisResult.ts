//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SynthesisStatus } from "./Exports";

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 * @class
 */
export class TranslationSynthesisResult {
    /**
     * Translated text in the target language.
     * @property
     * @returns Translated text in the target language.
     */
    public audio: Uint8Array;

    /**
     * Status of the synthesis.
     * @property
     * @returns status of the synthesis.
     */
    public synthesisStatus: SynthesisStatus;

    /**
     * Should the synthesis fail, the reason why.
     * @property
     */
    public failureReason: string;
}
