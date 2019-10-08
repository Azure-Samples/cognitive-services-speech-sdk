package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import java.util.ArrayList;
import java.util.List;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.internal.StringVector;
import com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfigPtrVector;

/**
 * Represents auto detect source language configuration used for specifying the possible source language candidates
 * Added in version 1.8.0
 */
public final class AutoDetectSourceLanguageConfig implements Closeable
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }
 
     /**
     * Creates an instance of the AutoDetectSourceLanguageConfig
     * @param languages Specifies the source language candidates.
     * This functionality was added in version 1.8.0.
     * @return The auto detect source language configuration being created.
     */
    public static AutoDetectSourceLanguageConfig fromLanguages(List<String> languages) {
        if (languages == null || languages.size() == 0) {
            throw new IllegalArgumentException("languages cannot be null or empty");
        }

        StringVector languageVector = new StringVector();
        for (String language : languages) {
            Contracts.throwIfIllegalLanguage(language, "invalid language value");
            languageVector.add(language);
        }

        return new AutoDetectSourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.AutoDetectSourceLanguageConfig.FromLanguages(languageVector));
    }

     /**
     * Creates an instance of the AutoDetectSourceLanguageConfig with a list of SourceLanguageConfig
     * @param sourceLanguageConfigs Specifies the source language config list
     * This functionality was added in version 1.8.0.
     * @return The AutoDetectSourceLanguageConfig being created.
     */
    public static AutoDetectSourceLanguageConfig fromSourceLanguageConfigs(List<SourceLanguageConfig> sourceLanguageConfigs) {
         if (sourceLanguageConfigs == null || sourceLanguageConfigs.size() == 0) {
            throw new IllegalArgumentException("sourceLanguageConfigs cannot be null or empty");
        }

        SourceLanguageConfigPtrVector sourceLanguageConfigPtrVector = new SourceLanguageConfigPtrVector();
        for (SourceLanguageConfig sourceLanguageConfig : sourceLanguageConfigs) {
            Contracts.throwIfNull(sourceLanguageConfig, "sourceLanguageConfig cannot be null");
            sourceLanguageConfigPtrVector.add(sourceLanguageConfig.getImpl());
        }
        return new AutoDetectSourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.AutoDetectSourceLanguageConfig.FromSourceLanguageConfigs(sourceLanguageConfigPtrVector));
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }
        this._configImpl.delete();
        disposed = true;
    }

    /*! \cond INTERNAL */
    /**
     * Returns the AutoDetectSourceLanguageConfig
     * @return The implementation of the AutoDetectSourceLanguageConfig object
     */
    public com.microsoft.cognitiveservices.speech.internal.AutoDetectSourceLanguageConfig getImpl()
    {
        return _configImpl;
    }
    /*! \endcond */

    private AutoDetectSourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.AutoDetectSourceLanguageConfig config) {
        Contracts.throwIfNull(config, "config");
        this._configImpl = config;
    }

    private com.microsoft.cognitiveservices.speech.internal.AutoDetectSourceLanguageConfig _configImpl;
    private boolean disposed = false;
}
