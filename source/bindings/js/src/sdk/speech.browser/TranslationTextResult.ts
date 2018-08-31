//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { ISpeechProperties, TranslationStatus } from "./Exports";

export class TranslationTextResult {
    /**
     * Specifies translation status.
     * @return the translation status.
     */
    public translationStatus: TranslationStatus;

    /**
     * Presents the translation results. Each item in the dictionary represents translation result in one of target languages, where the key
     * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
     * @return the current translation map.
     */
    public translations: ISpeechProperties;
}
