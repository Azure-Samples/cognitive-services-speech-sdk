//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines speech property ids.
 */
export enum PropertyId {

    /**
     * Subscription key.
     */
    SpeechServiceConnection_Key = 0,

    /**
     * Endpoint.
     */
    SpeechServiceConnection_Endpoint,

    /**
     * Region.
     */
    SpeechServiceConnection_Region,

    /**
     * Authorization token.
     */
    SpeechServiceAuthorization_Token,

    /**
     * Authorization type.
     */
    SpeechServiceAuthorization_Type,

    /**
     * RPS token.
     */
    SpeechServiceRps_Token,

    /**
     * Endpoint ID.
     */
    SpeechServiceConnection_EndpointId,

    /**
     * Translation from language.
     */
    SpeechServiceConnection_TranslationFromLanguage,

    /**
     * Translation to languages.
     */
    SpeechServiceConnection_TranslationToLanguages,

    /**
     * Translation output voice.
     */
    SpeechServiceConnection_TranslationVoice,

    /**
     * Translation features.
     */
    SpeechServiceConnection_TranslationFeatures,

    /**
     * Intent region.
     */
    SpeechServiceConnection_IntentRegion,

    /**
     * Intent source language. TODO:REMOVE
     */
    SpeechServiceConnection_IntentSourceLanguage,

    /**
     * Recognition mode.
     */
    SpeechServiceConnection_RecoMode,

    /**
     * Interactive mode. TODO:REMOVE
     */
    SpeechServiceConnection_RecoMode_Interactive,

    /**
     * Conversation mode. TODO:REMOVE
     */
    SpeechServiceConnection_RecoMode_Conversation,

    /**
     * Dictation mode. TODO:REMOVE
     */
    SpeechServiceConnection_RecoMode_Dictation,

    /**
     * Recognition language.
     */
    SpeechServiceConnection_RecoLanguage,

    /**
     * Session id. TODO: REMOVE
     */
    Speech_SessionId,

    /**
     * Detailed result required.
     */
    SpeechServiceResponse_RequestDetailedResultTrueFalse,

    /**
     * Profanity filtering required.
     */
    SpeechServiceResponse_RequestProfanityFilterTrueFalse,

    /**
     * TODO:WHAT IS THIS FOR?
     */
    SpeechServiceResponse_Json,

    /**
     * JSON in result.
     */
    SpeechServiceResponse_JsonResult,

    /**
     * Error details.
     */
    SpeechServiceResponse_JsonErrorDetails,

    /**
     * Cancellation reason.
     */
    CancellationDetails_Reason,

    /**
     * Cancellation text.
     */
    CancellationDetails_ReasonText,

    /**
     * Cancellation detailed text.
     */
    CancellationDetails_ReasonDetailedText,
}
