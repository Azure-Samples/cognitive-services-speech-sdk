//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines configurations for dialog connector.
 */
public class DialogConfig extends SpeechConfig  {

    /*! \cond PROTECTED */
    static Class<?> dialogConfig = null;

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        dialogConfig = DialogConfig.class;
    }

    protected DialogConfig(com.microsoft.cognitiveservices.speech.internal.DialogConfig dialogConfigImpl) {
        super(dialogConfigImpl);
    }
    /*! \endcond */

    /**
     * Creates an instance of the dialog config with the specified bot secret key.
     * @param botSecret Speech channel bot secret key.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @return The created config.
     */
    public static DialogConfig fromBotSecret(String botSecret, String subscription, String region) {
        Contracts.throwIfNull(botSecret, "botSecret");
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new DialogConfig(com.microsoft.cognitiveservices.speech.internal.DialogConfig.FromBotSecret(botSecret, subscription, region));
    }

    /**
     * Creates an instance of the dialog config with the specified Task Dialog Application Id.
     * @param appId Task Dialog Application Id.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @return The created config.
     */
    public static DialogConfig fromTaskDialogAppId(String appId, String subscription, String region) {
        Contracts.throwIfNull(appId, "appId");
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new DialogConfig(com.microsoft.cognitiveservices.speech.internal.DialogConfig.FromTaskDialogAppId(appId, subscription, region));
    }

    /**
     * Sets the Speech channel secret key.
     * @param value Speech channel bot secret key to set.
     */
    public void setBotSecret(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        getConfigImpl().SetSecretKey(value);
    }

    /**
     * Gets the Speech channel secret key.
     * @return Speech channel bot secret key.
     */
    public String getBotSecret() {
        return getConfigImpl().GetSecretKey();
    }

    /**
     * Sets the Task Dialog Application Id.
     * @param value Task Dialog Application Id to set.
     */
    public void setTaskDialogAppId(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        getConfigImpl().SetTaskDialogAppId(value);
    }

    /**
     * Gets the Task Dialog Application Id.
     * @return Task Dialog Application Id.
     */
    public String getTaskDialogAppId() {
        return getConfigImpl().GetTaskDialogAppId();
    }

    /**
     * Sets the audio format that is returned by text to speech during a reply from the backing dialog.
     * @param value The audio format to return.
     */
    public void setTextToSpeechAudioFormat(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        getConfigImpl().SetTextToSpeechAudioFormat(value);
    }

    /**
     * Gets the audio format that is returned by text to speech during a reply from the backing dialog.
     * @return The audio format.
     */
    public void getTextToSpeechAudioFormat() {
        getConfigImpl().GetTextToSpeechAudioFormat();
    }

    /**
      * Returns the dialog configuration.
      * @return The implementation of the config.
      */
    protected com.microsoft.cognitiveservices.speech.internal.DialogConfig getConfigImpl() {
        return (com.microsoft.cognitiveservices.speech.internal.DialogConfig) getImpl();
    }
}
