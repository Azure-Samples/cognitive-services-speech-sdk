//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import { PropertyCollection, ResultReason, SpeechRecognitionResult, Translation } from "./Exports";

/**
 * Translation text result.
 * @class TranslationRecognitionResult
 */
export class TranslationRecognitionResult extends SpeechRecognitionResult {
    private privTranslations: Translation;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {Translation} translations - The translations.
     * @param {string} resultId - The result id.
     * @param {ResultReason} reason - The reason.
     * @param {string} text - The recognized text.
     * @param {number} duration - The duration.
     * @param {number} offset - The offset into the stream.
     * @param {string} errorDetails - Error details, if provided.
     * @param {string} json - Additional Json, if provided.
     * @param {PropertyCollection} properties - Additional properties, if provided.
     */
    public constructor(translations: Translation, resultId?: string, reason?: ResultReason, text?: string, duration?: number, offset?: number, errorDetails?: string, json?: string, properties?: PropertyCollection) {
        super(resultId, reason, text, duration, offset, errorDetails, json, properties);

        this.privTranslations = translations;
    }

    /**
     * Presents the translation results. Each item in the dictionary represents a translation result in one of target languages, where the key
     * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
     * @member TranslationRecognitionResult.prototype.translations
     * @returns the current translation map that holds all translations requested.
     */
    public get translations(): Translation {
        return this.privTranslations;
    }
}
