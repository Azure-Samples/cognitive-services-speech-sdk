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
     * @param translations The translations.
     * @param resultId The result id.
     * @param reason The reason.
     * @param text The recognized text.
     * @param duration The duration.
     * @param offset The offset into the stream.
     * @param errorDetails Error details, if provided.
     * @param json Additional Json, if provided.
     * @param properties Additional properties, if provided.
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
