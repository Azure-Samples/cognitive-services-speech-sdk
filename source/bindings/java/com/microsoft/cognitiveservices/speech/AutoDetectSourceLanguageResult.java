package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents the result of auto detecting source languages
 * Added in version 1.8.0
 */
public final class AutoDetectSourceLanguageResult
{
     /**
     * Creates an instance of the AutoDetectSourceLanguageResult from a speech recognition result
     * @param speechRecognitionResult Specifies the speech recognition result
     * This functionality was added in version 1.8.0.
     * @return The AutoDetectSourceLanguageResult being created.
     */
    public static AutoDetectSourceLanguageResult fromResult(SpeechRecognitionResult speechRecognitionResult) {
        Contracts.throwIfNull(speechRecognitionResult, "speechRecognitionResult cannot be null");
        String language = speechRecognitionResult.getProperties().getProperty(PropertyId.SpeechServiceConnection_AutoDetectSourceLanguageResult);
        return new AutoDetectSourceLanguageResult(language);
    }

     /**
     * Get the language value
     * @return The language that is the result of auto detection
     */
    public String getLanguage() {
        return _language;
    }

    private AutoDetectSourceLanguageResult(String language) {
        _language = language;
    }

    private String _language;
}
