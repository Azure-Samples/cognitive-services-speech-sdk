//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
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
     * Recognition mode.
     */
    SpeechServiceConnection_RecoMode,

    /**
     * Recognition language.
     */
    SpeechServiceConnection_RecoLanguage,

    /**
     * Session id.
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

    /**
     * JSON result of language understanding service.
     */
    LanguageUnderstandingServiceResponse_JsonResult,
}
