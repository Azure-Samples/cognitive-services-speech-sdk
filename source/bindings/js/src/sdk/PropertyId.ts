// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
/**
 * Defines speech property ids.
 * @class PropertyId
 */
export enum PropertyId {

    /**
     * Subscription key.
     * @member PropertyId.SpeechServiceConnection_Key
     */
    SpeechServiceConnection_Key = 0,

    /**
     * Endpoint.
     * @member PropertyId.SpeechServiceConnection_Endpoint
     */
    SpeechServiceConnection_Endpoint,

    /**
     * Region.
     * @member PropertyId.SpeechServiceConnection_Region
     */
    SpeechServiceConnection_Region,

    /**
     * Authorization token.
     * @member PropertyId.SpeechServiceAuthorization_Token
     */
    SpeechServiceAuthorization_Token,

    /**
     * Authorization type.
     * @member PropertyId.SpeechServiceAuthorization_Type
     */
    SpeechServiceAuthorization_Type,

    /**
     * Endpoint ID.
     * @member PropertyId.SpeechServiceConnection_EndpointId
     */
    SpeechServiceConnection_EndpointId,

    /**
     * Translation to languages.
     * @member PropertyId.SpeechServiceConnection_TranslationToLanguages
     */
    SpeechServiceConnection_TranslationToLanguages,

    /**
     * Translation output voice.
     * @member PropertyId.SpeechServiceConnection_TranslationVoice
     */
    SpeechServiceConnection_TranslationVoice,

    /**
     * Translation features.
     * @member PropertyId.SpeechServiceConnection_TranslationFeatures
     */
    SpeechServiceConnection_TranslationFeatures,

    /**
     * Intent region.
     * @member PropertyId.SpeechServiceConnection_IntentRegion
     */
    SpeechServiceConnection_IntentRegion,

    /**
     * Recognition mode.
     * @member PropertyId.SpeechServiceConnection_RecoMode
     */
    SpeechServiceConnection_RecoMode,

    /**
     * Recognition language.
     * @member PropertyId.SpeechServiceConnection_RecoLanguage
     */
    SpeechServiceConnection_RecoLanguage,

    /**
     * Session id.
     * @member PropertyId.Speech_SessionId
     */
    Speech_SessionId,

    /**
     * Detailed result required.
     * @member PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse
     */
    SpeechServiceResponse_RequestDetailedResultTrueFalse,

    /**
     * Profanity filtering required.
     * @member PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse
     */
    SpeechServiceResponse_RequestProfanityFilterTrueFalse,

    /**
     * JSON in result.
     * @member PropertyId.SpeechServiceResponse_JsonResult
     */
    SpeechServiceResponse_JsonResult,

    /**
     * Error details.
     * @member PropertyId.SpeechServiceResponse_JsonErrorDetails
     */
    SpeechServiceResponse_JsonErrorDetails,

    /**
     * Cancellation reason.
     * @member PropertyId.CancellationDetails_Reason
     */
    CancellationDetails_Reason,

    /**
     * Cancellation text.
     * @member PropertyId.CancellationDetails_ReasonText
     */
    CancellationDetails_ReasonText,

    /**
     * Cancellation detailed text.
     * @member PropertyId.CancellationDetails_ReasonDetailedText
     */
    CancellationDetails_ReasonDetailedText,

    /**
     * JSON result of language understanding service.
     * @member PropertyId.LanguageUnderstandingServiceResponse_JsonResult
     */
    LanguageUnderstandingServiceResponse_JsonResult,
}
