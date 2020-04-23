//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.intent;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;

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
    public static LanguageUnderstandingModel fromEndpoint(String uri) {
        Contracts.throwIfNullOrWhitespace(uri, "uri");
        IntRef luModel = new IntRef(0);
        Contracts.throwIfFail(createModelFromUri(luModel, uri));
        return new LanguageUnderstandingModel(luModel);
    }

    /**
     * Creates a language understanding model using the application id of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @return The language understanding model being created.
     */
    public static LanguageUnderstandingModel fromAppId(String appId) {
        Contracts.throwIfNullOrWhitespace(appId, "appId");
        IntRef luModel = new IntRef(0);
        Contracts.throwIfFail(createModelFromAppId(luModel, appId));
        return new LanguageUnderstandingModel(luModel);
    }

    /**
     * Creates a language understanding model using subscription key, application id and region of Language Understanding service.
     * @param subscriptionKey A String that represents the subscription key of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @param region A String that represents the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @return The language understanding model being created.
     */
    public static LanguageUnderstandingModel fromSubscription(String subscriptionKey, String appId, String region) {
        Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts.throwIfNullOrWhitespace(appId, "appId");
        Contracts.throwIfNullOrWhitespace(region, "region");

        IntRef luModel = new IntRef(0);
        Contracts.throwIfFail(createModelFromSubscription(luModel, subscriptionKey, appId, region));
        return new LanguageUnderstandingModel(luModel);
    }

    LanguageUnderstandingModel(IntRef model) {
        Contracts.throwIfNull(model, "model");
        modelHandle = new SafeHandle(model.getValue(), SafeHandleType.LanguageUnderstandingModel);
    }

    /*! \cond INTERNAL */

    /**
     * Returns the language understanding model.
     * @return The implementation of the model.
     */
    public SafeHandle getImpl() {
        return modelHandle;
    }

    /*! \endcond */

    private SafeHandle modelHandle = null;
    private final static native long createModelFromUri(IntRef luModel, String uri);
    private final static native long createModelFromAppId(IntRef luModel, String appId);
    private final static native long createModelFromSubscription(IntRef luModel, String subscriptionKey, String appId, String region);
}
