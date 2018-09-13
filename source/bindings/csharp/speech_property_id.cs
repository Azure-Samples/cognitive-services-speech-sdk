//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines speech property ids.
    /// </summary>
    public enum SpeechPropertyId
    {
        /// <summary>
        /// Subscription key.
        /// </summary>
        SpeechServiceConnection_Key = Internal.SpeechPropertyId.SpeechServiceConnection_Key,

        /// <summary>
        /// Endpoint.
        /// </summary>
        SpeechServiceConnection_Endpoint = Internal.SpeechPropertyId.SpeechServiceConnection_Endpoint,

        /// <summary>
        /// Region.
        /// </summary>
        SpeechServiceConnection_Region = Internal.SpeechPropertyId.SpeechServiceConnection_Region,

        /// <summary>
        /// Authorization token.
        /// </summary>
        SpeechServiceAuthorization_Token = Internal.SpeechPropertyId.SpeechServiceAuthorization_Token,

        /// <summary>
        /// Authorization type.
        /// </summary>
        SpeechServiceAuthorization_Type = Internal.SpeechPropertyId.SpeechServiceAuthorization_Type,

        /// <summary>
        /// RPS token.
        /// </summary>
        SpeechServiceRps_Token = Internal.SpeechPropertyId.SpeechServiceRps_Token,

        /// <summary>
        /// Endpoint ID.
        /// </summary>
        SpeechServiceConnection_EndpointId = Internal.SpeechPropertyId.SpeechServiceConnection_EndpointId,

        /// <summary>
        /// Translation from language.
        /// </summary>
        SpeechServiceConnection_TranslationFromLanguage = Internal.SpeechPropertyId.SpeechServiceConnection_TranslationFromLanguage,

        /// <summary>
        /// Translation to languages.
        /// </summary>
        SpeechServiceConnection_TranslationToLanguages = Internal.SpeechPropertyId.SpeechServiceConnection_TranslationToLanguages,

        /// <summary>
        /// Translation output voice.
        /// </summary>
        SpeechServiceConnection_TranslationVoice = Internal.SpeechPropertyId.SpeechServiceConnection_TranslationVoice,

        /// <summary>
        /// Translation features.
        /// </summary>
        SpeechServiceConnection_TranslationFeatures = Internal.SpeechPropertyId.SpeechServiceConnection_TranslationFeatures,

        /// <summary>
        /// Intent region.
        /// </summary>
        SpeechServiceConnection_IntentRegion = Internal.SpeechPropertyId.SpeechServiceConnection_IntentRegion,

        /// <summary>
        /// Intent source language. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_IntentSourceLanguage = Internal.SpeechPropertyId.SpeechServiceConnection_IntentSourceLanguage,

        /// <summary>
        /// Recognition mode.
        /// </summary>
        SpeechServiceConnection_RecoMode = Internal.SpeechPropertyId.SpeechServiceConnection_RecoMode,

        /// <summary>
        /// Interactive mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Interactive = Internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Interactive,

        /// <summary>
        /// Conversation mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Conversation = Internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Conversation,

        /// <summary>
        /// Dictation mode. TODO:REMOVE
        /// </summary>
        SpeechServiceConnection_RecoMode_Dictation = Internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Dictation,

        /// <summary>
        /// Recognition language.
        /// </summary>
        SpeechServiceConnection_RecoLanguage = Internal.SpeechPropertyId.SpeechServiceConnection_RecoLanguage,

        /// <summary>
        /// Session id. TODO: REMOVE
        /// </summary>
        Speech_SessionId = Internal.SpeechPropertyId.Speech_SessionId,

        /// <summary>
        /// Detailed result required.
        /// </summary>
        SpeechServiceResponse_RequestDetailedResultTrueFalse = Internal.SpeechPropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse,

        /// <summary>
        /// Profanity filtering required.
        /// </summary>
        SpeechServiceResponse_RequestProfanityFilterTrueFalse = Internal.SpeechPropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse,

        /// <summary>
        /// TODO:WHAT IS THIS FOR?
        /// </summary>
        SpeechServiceResponse_Json = Internal.SpeechPropertyId.SpeechServiceResponse_Json,

        /// <summary>
        /// JSON in result.
        /// </summary>
        SpeechServiceResponse_JsonResult = Internal.SpeechPropertyId.SpeechServiceResponse_JsonResult,

        /// <summary>
        /// Error details.
        /// </summary>
        SpeechServiceResponse_JsonErrorDetails = Internal.SpeechPropertyId.SpeechServiceResponse_JsonErrorDetails,

        /// <summary>
        /// Cancellation reason.
        /// </summary>
        CancellationDetails_Reason = Internal.SpeechPropertyId.CancellationDetails_Reason,

        /// <summary>
        /// Cancellation text.
        /// </summary>
        CancellationDetails_ReasonText = Internal.SpeechPropertyId.CancellationDetails_ReasonText,

        /// <summary>
        /// Cancellation detailed text.
        /// </summary>
        CancellationDetails_ReasonDetailedText = Internal.SpeechPropertyId.CancellationDetails_ReasonDetailedText
    }
}
