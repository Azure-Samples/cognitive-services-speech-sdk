//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.util;

import java.io.Closeable;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/*! \cond INTERNAL */

/**
 * Internal SafeHandle class
 */

public class SafeHandle implements Closeable {

    public SafeHandle(long val, SafeHandleType type) {
        //System.out.println("Java SafeHandle constructor, value is: " + String.valueOf(val) + ", HandleType is: " + String.valueOf(type));
        value = val;
        handleType = type;
    }

    /*! \cond PROTECTED */

    @Override
    protected void finalize() throws Throwable {
        this.close();
    }

    /*! \endcond */

    /**
     * Sets the long value
     * @param newValue Long value to set.
     * @return
     */
    public void setValue(long newValue) {
        //System.out.println("Java SafeHandle setValue to " + String.valueOf(newValue) + ", HandleType is: " + String.valueOf(handleType));
        this.value = newValue;
    }

    /**
     * Gets the long value
     * @return long value
     */
    public long getValue() {
        return this.value;
    }

     /**
     * Dispose of associated resources.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
     */
    @Override
    public void close() {
        //System.out.println("Java SafeHandle close, value is: " + String.valueOf(value) + ", HandleType is: " + String.valueOf(handleType));
        if (value != 0) {
            if (handleType == SafeHandleType.Event) {
                releaseEventHandle(value);
            }
            else if (handleType == SafeHandleType.Recognizer) {
                releaseRecognizerHandle(value);
            }
            else if (handleType == SafeHandleType.SpeechConfig) {
                releaseSpeechConfigHandle(value);
            }
            else if (handleType == SafeHandleType.PropertyCollection) {
                releasePropertyHandle(value);
            }
            else if (handleType == SafeHandleType.RecognitionResult) {
                releaseRecognizerResultHandle(value);
            }
            else if (handleType == SafeHandleType.AudioConfig) {
                releaseAudioConfigHandle(value);
            }
            else if (handleType == SafeHandleType.Connection) {
                releaseConnectionHandle(value);
            }
            else if (handleType == SafeHandleType.AudioInputStream) {
                releaseAudioInputStreamHandle(value);
            }
            else if (handleType == SafeHandleType.AudioStreamFormat) {
                releaseAudioStreamFormatHandle(value);
            }
            else if (handleType == SafeHandleType.KeywordModel) {
                releaseKeywordModelHandle(value);
            }
            else if (handleType == SafeHandleType.LanguageUnderstandingModel) {
                releaseLanguageUnderstandingModelHandle(value);
            }
            else if (handleType == SafeHandleType.IntentTrigger) {
                releaseIntentTriggerHandle(value);
            }
            else if (handleType == SafeHandleType.User) {
                releaseUserHandle(value);
            }
            else if (handleType == SafeHandleType.Participant) {
                releaseParticipantHandle(value);
            }
            else if (handleType == SafeHandleType.Conversation) {
                releaseConversationHandle(value);
            }
            else if (handleType == SafeHandleType.DialogServiceConnector) {
                releaseDialogServiceConnectorHandle(value);
            }
            else if (handleType == SafeHandleType.DialogServiceConnectorEvent) {
                releaseDialogServiceConnectorEventHandle(value);
            }
            else if (handleType == SafeHandleType.AudioOutputStream) {
                releaseAudioOutputStreamHandle(value);
            }
            else if (handleType == SafeHandleType.AutoDetectSourceLanguageConfig) {
                releaseAutoDetectSourceLanguageConfigHandle(value);
            }            
            else if (handleType == SafeHandleType.SourceLanguageConfig) {
                releaseSourceLanguageConfigHandle(value);
            }
            else if (handleType == SafeHandleType.SynthesisEvent) {
                releaseSynthesisEventHandle(value);
            }
            else if (handleType == SafeHandleType.SynthesisResult) {
                releaseSynthesisResultHandle(value);
            }
            else if (handleType == SafeHandleType.AudioDataStream) {
                releaseAudioDataStreamHandle(value);
            }
            else if (handleType == SafeHandleType.Synthesizer) {
                releaseSynthesizerHandle(value);
            }
            else if (handleType == SafeHandleType.Grammar) {
                releaseGrammarHandle(value);
            }
            else if (handleType == SafeHandleType.TranslationSynthesis) {
                releaseTranslationSynthesisHandle(value);
            }
            else if (handleType == SafeHandleType.KeywordRecognizer) {
                releaseKeywordRecognizerHandle(value);
            }
            else {
                System.out.println("Java SafeHandle close, invalid handle value: " + String.valueOf(value));
            }
            value = 0;
            handleType = SafeHandleType.UnInitialized;
        }
    }

    private final native long releaseEventHandle(long handle);
    private final native long releaseRecognizerHandle(long handle);
    private final native long releaseSpeechConfigHandle(long handle);
    private final native long releaseAudioConfigHandle(long handle);
    private final native long releasePropertyHandle(long handle);
    private final native long releaseRecognizerResultHandle(long handle);
    private final native long releaseConnectionHandle(long handle);
    private final native long releaseAudioInputStreamHandle(long handle);
    private final native long releaseAudioOutputStreamHandle(long handle);
    private final native long releaseAudioStreamFormatHandle(long handle);
    private final native long releaseKeywordModelHandle(long handle);
    private final native long releaseLanguageUnderstandingModelHandle(long handle);
    private final native long releaseIntentTriggerHandle(long handle);
    private final native long releaseUserHandle(long handle);
    private final native long releaseParticipantHandle(long handle);
    private final native long releaseConversationHandle(long handle);
    private final native long releaseDialogServiceConnectorHandle(long handle);
    private final native long releaseDialogServiceConnectorEventHandle(long handle);
    private final native long releaseAutoDetectSourceLanguageConfigHandle(long handle);
    private final native long releaseSourceLanguageConfigHandle(long handle);
    private final native long releaseSynthesisEventHandle(long handle);
    private final native long releaseSynthesisResultHandle(long handle);
    private final native long releaseAudioDataStreamHandle(long handle);
    private final native long releaseSynthesizerHandle(long handle);
    private final native long releaseGrammarHandle(long handle);
    private final native long releaseTranslationSynthesisHandle(long handle);
    private final native long releaseKeywordRecognizerHandle(long handle);

    private long value = 0;
    private SafeHandleType handleType = SafeHandleType.UnInitialized;
}

/*! \endcond */