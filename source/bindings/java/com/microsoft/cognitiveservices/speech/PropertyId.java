//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/// <summary>
/// Defines property ids.
/// </summary>
public enum PropertyId {

    /// <summary>
    /// Subscription key.
    /// </summary>
    SpeechServiceConnection_Key (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Key),

    /// <summary>
    /// Endpoint.
    /// </summary>
    SpeechServiceConnection_Endpoint (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Endpoint),

    /// <summary>
    /// Region.
    /// </summary>
    SpeechServiceConnection_Region (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Region),

    /// <summary>
    /// Authorization token.
    /// </summary>
    SpeechServiceAuthorization_Token (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Token),

    /// <summary>
    /// Authorization type.
    /// </summary>
    SpeechServiceAuthorization_Type (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Type),

    /// <summary>
    /// Endpoint ID.
    /// </summary>
    SpeechServiceConnection_EndpointId (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EndpointId),

    /// <summary>
    /// Translation to languages.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationToLanguages),

    /// <summary>
    /// Translation output voice.
    /// </summary>
    SpeechServiceConnection_TranslationVoice (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationVoice),

    /// <summary>
    /// Translation features.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationFeatures),

    /// <summary>
    /// Intent region.
    /// </summary>
    SpeechServiceConnection_IntentRegion (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_IntentRegion),

    /// <summary>
    /// Recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
    /// </summary>
    SpeechServiceConnection_RecoMode (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode),

    /// <summary>
    /// Recognition language.
    /// </summary>
    SpeechServiceConnection_RecoLanguage ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoLanguage),

    /// <summary>
    /// Session ID.
    /// </summary>
    Speech_SessionId ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Speech_SessionId),

    /// <summary>
    /// Detailed result required.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse),

    /// <summary>
    /// Profanity filtering required.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse),

    /// <summary>
    /// JSON result of speech recognition service.
    /// </summary>
    SpeechServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonResult),

    /// <summary>
    /// Error details.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonErrorDetails),

    /// <summary>
    /// Cancellation reason.
    /// </summary>
    CancellationDetails_Reason ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_Reason),

    /// <summary>
    /// Cancellation text.
    /// </summary>
    CancellationDetails_ReasonText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonText),

    /// <summary>
    /// Cancellation detailed text.
    /// </summary>
    CancellationDetails_ReasonDetailedText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonDetailedText),

    /// <summary>
    /// JSON result of language understanding service.
    /// </summary>
    LanguageUnderstandingServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.LanguageUnderstandingServiceResponse_JsonResult);

    private PropertyId(com.microsoft.cognitiveservices.speech.internal.PropertyId id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @param
     * @return the speech property id
     */
    public com.microsoft.cognitiveservices.speech.internal.PropertyId getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.PropertyId id;
}
