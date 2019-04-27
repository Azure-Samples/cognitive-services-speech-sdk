//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines configurations for speech bot connector.
 */
public class BotConnectorConfig extends SpeechConfig  {

    /*! \cond PROTECTED */
    static Class<?> botConnectorConfig = null;

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        botConnectorConfig = BotConnectorConfig.class;
    }

    protected BotConnectorConfig(com.microsoft.cognitiveservices.speech.internal.BotConnectorConfig botConnectorConfigImpl) {
        super(botConnectorConfigImpl);
    }
    /*! \endcond */

    /**
     * Creates and instance of the bot connector config with the specified secret key.
     * @param secretKey Speech channel secret key.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @return The created config.
     */
    public static BotConnectorConfig fromSecretKey(String secretKey, String subscription, String region) {
        Contracts.throwIfNull(secretKey, "secretKey");
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new BotConnectorConfig(com.microsoft.cognitiveservices.speech.internal.BotConnectorConfig.FromSecretKey(secretKey, subscription, region));
    }

    /**
     * Sets the Speech channel secret key..
     * @param value Speech channel secret key to set.
     */
    public void setSecretKey(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        getBotConfigImpl().SetSecretKey(value);
    }

    /**
     * Gets the Speech channel secret key.
     * @return Speech channel secret key.
     */
    public String getSecretKey() {
        return getBotConfigImpl().GetSecretKey();
    }

    /**
     * Sets the audio format that is returned by text to speech during a bot reply.
     * @param value The audio format to return.
     */
    public void setBotTextToSpeechAudioFormat(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        getBotConfigImpl().SetTextToSpeechAudioFormat(value);
    }

    /**
     * Gets the audio format that is returned by text to speech during a bot reply.
     * @return The audio format.
     */
    public void getBotTextToSpeechAudioFormat() {
        getBotConfigImpl().GetTextToSpeechAudioFormat();
    }

    /**
      * Returns the bot connector configuration.
      * @return The implementation of the config.
      */
    protected com.microsoft.cognitiveservices.speech.internal.BotConnectorConfig getBotConfigImpl() {
        return (com.microsoft.cognitiveservices.speech.internal.BotConnectorConfig) getImpl();
    }
}
