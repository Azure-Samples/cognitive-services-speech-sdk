//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines speech property ids.
    /// Changed in version 1.1.0.
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
        /// Endpoint ID.
        /// </summary>
        SpeechServiceConnection_EndpointId = Internal.PropertyId.SpeechServiceConnection_EndpointId,

        /// <summary>
        /// The host name of the proxy server.
        /// Added in version 1.1.0
        /// </summary>
        SpeechServiceConnection_ProxyHostName = Internal.PropertyId.SpeechServiceConnection_ProxyHostName,

        /// <summary>
        /// The port of the proxy server.
        /// Added in version 1.1.0
        /// </summary>
        SpeechServiceConnection_ProxyPort  = Internal.PropertyId.SpeechServiceConnection_ProxyPort,

        /// <summary>
        /// The user name of the proxy server.
        /// Added in version 1.1.0
        /// </summary>
        SpeechServiceConnection_ProxyUserName  = Internal.PropertyId.SpeechServiceConnection_ProxyUserName,

        /// <summary>
        /// The password of the proxy server.
        /// Added in version 1.1.0
        /// </summary>
        SpeechServiceConnection_ProxyPassword = Internal.PropertyId.SpeechServiceConnection_ProxyPassword,

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
        /// Recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
        /// </summary>
        SpeechServiceConnection_RecoMode = Internal.PropertyId.SpeechServiceConnection_RecoMode,

        /// <summary>
        /// Recognition language.
        /// </summary>
        SpeechServiceConnection_RecoLanguage = Internal.PropertyId.SpeechServiceConnection_RecoLanguage,

        /// <summary>
        /// Session ID.
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
        /// JSON result of speech recognition service.
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
        CancellationDetails_ReasonDetailedText = Internal.PropertyId.CancellationDetails_ReasonDetailedText,

        /// <summary>
        /// JSON result of language understanding service.
        /// </summary>
        LanguageUnderstandingServiceResponse_JsonResult = Internal.PropertyId.LanguageUnderstandingServiceResponse_JsonResult,
    }
}
