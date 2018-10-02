//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents language understanding model used for intent recognition.
 */
public final class LanguageUnderstandingModel
{
    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            // ignored.
        }
    }

    /**
     * Creates a language understanding model using the specified endpoint.
     * @param uri A String that represents the endpoint of the language understanding model.
     * @return The language understanding model being created.
     */
    public static LanguageUnderstandingModel fromEndpoint(String uri)
    {
        Contracts.throwIfNullOrWhitespace(uri, "uri");

        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromEndpoint(uri));
    }

    /**
     * Creates a language understanding model using the application id of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @return The language understanding model being created.
     */
    public static LanguageUnderstandingModel fromAppId(String appId)
    {
        Contracts.throwIfNullOrWhitespace(appId, "appId");

        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromAppId(appId));
    }

    /**
     * Creates a language understanding model using subscription key, application id and region of Language Understanding service.
     * @param subscriptionKey A String that represents the subscription key of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @param region A String that represents the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The language understanding model being created.
     */
    public static LanguageUnderstandingModel fromSubscription(String subscriptionKey, String appId, String region)
    {
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(appId, "appId");
        Contracts.throwIfNullOrWhitespace(region, "region");

        return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromSubscription(subscriptionKey, appId, region));
    }

    LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel model)
    {
        Contracts.throwIfNull(model, "model");

        modelImpl = model;
    }

    private com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel modelImpl;

    /*! \cond INTERNAL */

    /**
     * Returns the language understanding model.
     * @return The implementation of the model.
     */
    public com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel getModelImpl()
    {
        return modelImpl;
    }

    /*! \endcond */
}
