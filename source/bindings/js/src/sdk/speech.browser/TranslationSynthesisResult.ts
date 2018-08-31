//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { SynthesisStatus } from "./Exports";

/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 */
export class TranslationSynthesisResult {
    /**
     * Translated text in the target language.
     * @return Translated text in the target language.
     */
    public audio: ByteString;

    /**
     * Status of the synthesis.
     * @return status of the synthesis.
     */
    public synthesisStatus: SynthesisStatus;
}
