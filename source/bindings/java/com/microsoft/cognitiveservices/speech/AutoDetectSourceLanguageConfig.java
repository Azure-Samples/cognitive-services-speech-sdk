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
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

/**
 * Represents auto detect source language configuration used for specifying the possible source language candidates
 * Updated in version 1.13.0
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
     * Creates an instance of the AutoDetectSourceLanguageConfig with open range
     * Note: only <see cref="SpeechSynthesizer"/> supports source language auto detection from open range,
     * for <see cref="Recognizer"/>, please use AutoDetectSourceLanguageConfig with specific source languages.
     * Added in 1.13.0
     * @return The auto detect source language configuration being created.
     */
    public static AutoDetectSourceLanguageConfig fromOpenRange() {
        IntRef autoDetectLangConfigRef = new IntRef(0);
        Contracts.throwIfFail(fromOpenRange(autoDetectLangConfigRef));
        return new AutoDetectSourceLanguageConfig(autoDetectLangConfigRef.getValue());
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
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < languages.size(); i++) {
            sb.append(languages.get(i));
            // if not the last item
            if (i != languages.size() - 1) {
                sb.append(",");
            }
        }
        IntRef langConfigRef = new IntRef(0);
        Contracts.throwIfFail(fromLanguages(langConfigRef, sb.toString()));
        return new AutoDetectSourceLanguageConfig(langConfigRef.getValue());
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

        boolean isFirst = true;
        IntRef autoDetectLangConfigRef = new IntRef(0);
        for (SourceLanguageConfig sourceLanguageConfig : sourceLanguageConfigs) {
            Contracts.throwIfNull(sourceLanguageConfig, "sourceLanguageConfig cannot be null");
            if (isFirst == true)
            {
                Contracts.throwIfFail(createFromSourceLangConfig(autoDetectLangConfigRef, sourceLanguageConfig.getImpl()));
                isFirst = false;
            }
            else
            {
                Contracts.throwIfFail(addSourceLangConfigToAutoDetectSourceLangConfig(autoDetectLangConfigRef, sourceLanguageConfig.getImpl()));
            }
        }
        
        return new AutoDetectSourceLanguageConfig(autoDetectLangConfigRef.getValue());
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
     * Returns the AutoDetectSourceLanguageConfig
     * @return The implementation of the AutoDetectSourceLanguageConfig object
     */
    public SafeHandle getImpl()
    {
        return configHandle;
    }
    /*! \endcond */

    private AutoDetectSourceLanguageConfig(long handleValue) {
        Contracts.throwIfNull(handleValue, "handleValue");
        this.configHandle = new SafeHandle(handleValue, SafeHandleType.AutoDetectSourceLanguageConfig);
    }

    private final static native long fromOpenRange(IntRef langConfigRef);
    private final static native long fromLanguages(IntRef langConfigRef, String languages);
    private final static native long createFromSourceLangConfig(IntRef autoDetectLangConfigRef, SafeHandle sourceLanguageConfig);
    private final static native long addSourceLangConfigToAutoDetectSourceLangConfig(IntRef autoDetectLangConfigRef, SafeHandle sourceLanguageConfig);

    private SafeHandle configHandle = null;
    private boolean disposed = false;
}
