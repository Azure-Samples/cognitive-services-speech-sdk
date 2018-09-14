//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines speech property ids.
    /// </summary>
    public enum PropertyId
    {
        /// <summary>
        /// Subscription key.
        /// </summary>
        SpeechServiceConnection_Key = Internal.PropertyId.SpeechServiceConnection_Key,

        /// <summary>
        /// Endpoint.
        /// </summary>
        SpeechServiceConnection_Endpoint = Internal.PropertyId.SpeechServiceConnection_Endpoint,

        /// <summary>
        /// Region.
        /// </summary>
        SpeechServiceConnection_Region = Internal.PropertyId.SpeechServiceConnection_Region,

        /// <summary>
        /// Authorization token.
        /// </summary>
        SpeechServiceAuthorization_Token = Internal.PropertyId.SpeechServiceAuthorization_Token,

        /// <summary>
        /// Authorization type.
        /// </summary>
        SpeechServiceAuthorization_Type = Internal.PropertyId.SpeechServiceAuthorization_Type,

        /// <summary>
        /// RPS token.
        /// </summary>
        SpeechServiceRps_Token = Internal.PropertyId.SpeechServiceRps_Token,

        /// <summary>
        /// Endpoint ID.
        /// </summary>
        SpeechServiceConnection_EndpointId = Internal.PropertyId.SpeechServiceConnection_EndpointId,

        /// <summary>
        /// Translation from language.
        /// </summary>
        SpeechServiceConnection_TranslationFromLanguage = Internal.PropertyId.SpeechServiceConnection_TranslationFromLanguage,

        /// <summary>
        /// Translation to languages.
        /// </summary>
        SpeechServiceConnection_TranslationToLanguages = Internal.PropertyId.SpeechServiceConnection_TranslationToLanguages,

        /// <summary>
        /// Translation output voice.
        /// </summary>
        SpeechServiceConnection_TranslationVoice = Internal.PropertyId.SpeechServiceConnection_TranslationVoice,

        /// <summary>
        /// Translation features.
        /// </summary>
        SpeechServiceConnection_TranslationFeatures = Internal.PropertyId.SpeechServiceConnection_TranslationFeatures,

        /// <summary>
        /// Intent region.
        /// </summary>
        SpeechServiceConnection_IntentRegion = Internal.PropertyId.SpeechServiceConnection_IntentRegion,

        /// <summary>
        /// Intent source language. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_IntentSourceLanguage = Internal.PropertyId.SpeechServiceConnection_IntentSourceLanguage,

        /// <summary>
        /// Recognition mode.
        /// </summary>
        SpeechServiceConnection_RecoMode = Internal.PropertyId.SpeechServiceConnection_RecoMode,

        /// <summary>
        /// Interactive mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Interactive = Internal.PropertyId.SpeechServiceConnection_RecoMode_Interactive,

        /// <summary>
        /// Conversation mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Conversation = Internal.PropertyId.SpeechServiceConnection_RecoMode_Conversation,

        /// <summary>
        /// Dictation mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Dictation = Internal.PropertyId.SpeechServiceConnection_RecoMode_Dictation,

        /// <summary>
        /// Recognition language.
        /// </summary>
        SpeechServiceConnection_RecoLanguage = Internal.PropertyId.SpeechServiceConnection_RecoLanguage,

        /// <summary>
        /// Session id. TODO: REMOVE
        /// </summary>
        Speech_SessionId = Internal.PropertyId.Speech_SessionId,

        /// <summary>
        /// Detailed result required.
        /// </summary>
        SpeechServiceResponse_RequestDetailedResultTrueFalse = Internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse,

        /// <summary>
        /// Profanity filtering required.
        /// </summary>
        SpeechServiceResponse_RequestProfanityFilterTrueFalse = Internal.PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse,

        /// <summary>
        /// TODO:WHAT IS THIS FOR?
        /// </summary>
        SpeechServiceResponse_Json = Internal.PropertyId.SpeechServiceResponse_Json,

        /// <summary>
        /// JSON in result.
        /// </summary>
        SpeechServiceResponse_JsonResult = Internal.PropertyId.SpeechServiceResponse_JsonResult,

        /// <summary>
        /// Error details.
        /// </summary>
        SpeechServiceResponse_JsonErrorDetails = Internal.PropertyId.SpeechServiceResponse_JsonErrorDetails,

        /// <summary>
        /// Cancellation reason.
        /// </summary>
        CancellationDetails_Reason = Internal.PropertyId.CancellationDetails_Reason,

        /// <summary>
        /// Cancellation text.
        /// </summary>
        CancellationDetails_ReasonText = Internal.PropertyId.CancellationDetails_ReasonText,

        /// <summary>
        /// Cancellation detailed text.
        /// </summary>
        CancellationDetails_ReasonDetailedText = Internal.PropertyId.CancellationDetails_ReasonDetailedText
    }
}
