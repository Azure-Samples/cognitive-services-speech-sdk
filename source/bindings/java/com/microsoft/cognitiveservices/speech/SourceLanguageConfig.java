package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents source language configuration used for specifying recognition source language
 * Added in version 1.8.0
 */
public final class SourceLanguageConfig implements Closeable
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
     * Creates an instance of the SourceLanguageConfig
     * @param language Specifies the source language.
     * This functionality was added in version 1.8.0.
     * @return The source language configuration being created.
     */
    public static SourceLanguageConfig fromLanguage(String language) {
        Contracts.throwIfIllegalLanguage(language, "invalid language value");
        return new SourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfig.FromLanguage(language));
    }

     /**
     * Creates an instance of the SourceLanguageConfig with source language and endpoint ID.
     * @param language Specifies the source language.
     * @param endpointId Specifies the endpoint ID of a customized speech model that is used for speech recognition.
     * This functionality was added in version 1.8.0.
     * @return The source language configuration being created.
     */
    public static SourceLanguageConfig fromLanguage(String language, String endpointId) {
        Contracts.throwIfIllegalLanguage(language, "invalid language value");
        Contracts.throwIfNullOrWhitespace(endpointId, "endpointId cannot be empty");
        return new SourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfig.FromLanguage(language, endpointId));
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
     * Returns the implementation of source language config object
     * @return The implementation of the source language Config
     */
    public com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfig getImpl()
    {
        return this._configImpl;
    }
    /*! \endcond */

    private SourceLanguageConfig(com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfig config) {
        Contracts.throwIfNull(config, "config");
        this._configImpl = config;
    }

    private com.microsoft.cognitiveservices.speech.internal.SourceLanguageConfig _configImpl;
    private boolean disposed = false;
}
