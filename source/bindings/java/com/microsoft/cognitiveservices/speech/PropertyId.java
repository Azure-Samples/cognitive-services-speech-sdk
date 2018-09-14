package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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
    /// RPS token.
    /// </summary>
    SpeechServiceRps_Token (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceRps_Token),

    /// <summary>
    /// Endpoint ID.
    /// </summary>
    SpeechServiceConnection_EndpointId (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EndpointId),

    /// <summary>
    /// Translation from language.
    /// </summary>
    SpeechServiceConnection_TranslationFromLanguage (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationFromLanguage),

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
    /// Intent source language. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_IntentSourceLanguage (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_IntentSourceLanguage),

    /// <summary>
    /// Recognition mode.
    /// </summary>
    SpeechServiceConnection_RecoMode (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode),

    /// <summary>
    /// Interactive mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Interactive ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode_Interactive),

    /// <summary>
    /// Conversation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Conversation ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode_Conversation),

    /// <summary>
    /// Dictation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Dictation ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode_Dictation),

    /// <summary>
    /// Recognition language.
    /// </summary>
    SpeechServiceConnection_RecoLanguage ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoLanguage),

    /// <summary>
    /// Session id. TODO: REMOVE
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
    /// JSON in result.
    /// </summary>
    SpeechServiceResponse_Json ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_Json),

    /// <summary>
    /// JSON of language understanding in result.
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
    CancellationDetails_ReasonDetailedText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonDetailedText);

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
