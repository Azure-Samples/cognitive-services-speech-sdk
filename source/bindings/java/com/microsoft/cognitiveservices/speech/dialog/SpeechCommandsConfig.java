//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines configurations for the dialog service connector object for using a SpeechCommands backend.
 */
public final class SpeechCommandsConfig extends DialogServiceConfig {
    private SpeechCommandsConfig(com.microsoft.cognitiveservices.speech.internal.SpeechCommandsConfig speechCommandsConfigImpl) {
        super(speechCommandsConfigImpl);
    }

    /**
     * Creates an instance of speech commands config with the specified Speech Commands application id, subscription and region.
     * @param appId Speech Commands Application Id.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @return The created config.
     */
    public static SpeechCommandsConfig fromSubscription(String appId, String subscription, String region) {
        Contracts.throwIfNull(appId, "appId");
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new SpeechCommandsConfig(com.microsoft.cognitiveservices.speech.internal.SpeechCommandsConfig.FromSubscription(appId, subscription, region));
    }

    /**
     * Creates an instance of speech commands config with the specified Speech Commands application id, authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expires, the caller needs to refresh it by calling this setter with a new valid token.
     * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
     * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
     * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
     * @param appId Speech Commands Application Id.
     * @param authorizationToken The authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return A speech config instance.
     */
    public static SpeechCommandsConfig fromAuthorizationToken(String appId, String authorizationToken, String region) {
        Contracts.throwIfNull(appId, "appId");
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");
        return new SpeechCommandsConfig(com.microsoft.cognitiveservices.speech.internal.SpeechCommandsConfig.FromAuthorizationToken(appId, authorizationToken, region));
    }

    /**
     * Sets the application id.
     * @param value Identifier used to connect to the backend service.
     */
    public void setApplicationId(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        this.setProperty(PropertyId.Conversation_ApplicationId, value);
    }

    /**
     * Gets the application id.
     * @return Identifier used to connect to the backend service.
     */
    public String getApplicationId() {
        return this.getProperty(PropertyId.Conversation_ApplicationId);
    }
}





