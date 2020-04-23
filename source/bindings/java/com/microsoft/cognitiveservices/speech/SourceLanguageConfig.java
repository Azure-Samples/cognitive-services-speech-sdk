package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;

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
        IntRef langConfigRef = new IntRef(0);
        Contracts.throwIfFail(fromLanguage(langConfigRef, language));
        return new SourceLanguageConfig(langConfigRef.getValue());
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
        IntRef langConfigRef = new IntRef(0);
        Contracts.throwIfFail(fromLanguageAndEndpointId(langConfigRef, language, endpointId));
        return new SourceLanguageConfig(langConfigRef.getValue());
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if (disposed) {
            return;
        }
        if (configHandle != null){
            this.configHandle.close();
            this.configHandle = null;
        }

        disposed = true;
    }

    /*! \cond INTERNAL */

    /**
     * Returns the implementation of source language config object
     * @return The implementation of the source language Config
     */
    public SafeHandle getImpl()
    {
        return this.configHandle;
    }

    /*! \endcond */

    private SourceLanguageConfig(long handleValue) {
        Contracts.throwIfNull(handleValue, "handleValue");
        this.configHandle = new SafeHandle(handleValue, SafeHandleType.SourceLanguageConfig);
    }

    private final static native long fromLanguage(IntRef langConfigRef, String language);
    private final static native long fromLanguageAndEndpointId(IntRef langConfigRef, String language, String endpointId);

    private SafeHandle configHandle = null;
    private boolean disposed = false;
}
