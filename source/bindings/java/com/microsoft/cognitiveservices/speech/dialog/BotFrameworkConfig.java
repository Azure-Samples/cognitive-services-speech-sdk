//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;

import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Class that defines configurations for the dialog service connector object for using a Bot Framework backend.
 */
public final class BotFrameworkConfig extends DialogServiceConfig {
    private BotFrameworkConfig(com.microsoft.cognitiveservices.speech.internal.BotFrameworkConfig botFrameworkConfigImpl) {
        super(botFrameworkConfigImpl);
    }

    /**
     * Creates an instance of the bot framework config with the specified subscription and region.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @return The created config.
     */
    public static BotFrameworkConfig fromSubscription(String subscription, String region) {
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new BotFrameworkConfig(com.microsoft.cognitiveservices.speech.internal.BotFrameworkConfig.FromSubscription(subscription, region, ""));
    }

    /**
     * Creates an instance of the bot framework config with the specified subscription, region, and bot ID.
     * @param subscription Service subscription key.
     * @param region Service region key.
     * @param botId Optional, ID for using a specific bot.
     * @return The created config.
     */
    public static BotFrameworkConfig fromSubscription(String subscription, String region, String botId) {
        Contracts.throwIfNull(subscription, "subscription");
        Contracts.throwIfNull(region, "region");
        return new BotFrameworkConfig(com.microsoft.cognitiveservices.speech.internal.BotFrameworkConfig.FromSubscription(subscription, region, botId));
    }

    /**
     * Creates an instance of a bot framework config with specified authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expires, the caller needs to refresh it by calling this setter with a new valid token.
     * As configuration values are copied when creating a new recognizer, the new token value will not apply to recognizers that have already been created.
     * For recognizers that have been created before, you need to set authorization token of the corresponding recognizer
     * to refresh the token. Otherwise, the recognizers will encounter errors during recognition.
     * @param authorizationToken The authorization token.
     * @param region The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return A speech config instance.
     */
    public static BotFrameworkConfig fromAuthorizationToken(String authorizationToken, String region) {
        Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts.throwIfNullOrWhitespace(region, "region");
        return new BotFrameworkConfig(com.microsoft.cognitiveservices.speech.internal.BotFrameworkConfig.FromAuthorizationToken(authorizationToken, region));
    }
}
