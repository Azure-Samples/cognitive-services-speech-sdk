package com.microsoft.cognitiveservices.speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Defines speech property ids.
/// </summary>
public enum SpeechPropertyId {
    
    /// <summary>
    /// Subscription key.
    /// </summary>
    SpeechServiceConnection_Key (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_Key),

    /// <summary>
    /// Endpoint.
    /// </summary>
    SpeechServiceConnection_Endpoint (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_Endpoint),

    /// <summary>
    /// Region.
    /// </summary>
    SpeechServiceConnection_Region (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_Region),

    /// <summary>
    /// Authorization token.
    /// </summary>
    SpeechServiceAuthorization_Token (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceAuthorization_Token),

    /// <summary>
    /// Authorization type.
    /// </summary>
    SpeechServiceAuthorization_Type (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceAuthorization_Type),

    /// <summary>
    /// RPS token.
    /// </summary>
    SpeechServiceRps_Token (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceRps_Token),

    /// <summary>
    /// Endpoint ID.
    /// </summary>
    SpeechServiceConnection_EndpointId (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_EndpointId),

    /// <summary>
    /// Translation from language.
    /// </summary>
    SpeechServiceConnection_TranslationFromLanguage (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_TranslationFromLanguage),

    /// <summary>
    /// Translation to languages.
    /// </summary>
    SpeechServiceConnection_TranslationToLanguages (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_TranslationToLanguages),

    /// <summary>
    /// Translation output voice.
    /// </summary>
    SpeechServiceConnection_TranslationVoice (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_TranslationVoice),

    /// <summary>
    /// Translation features.
    /// </summary>
    SpeechServiceConnection_TranslationFeatures (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_TranslationFeatures),

    /// <summary>
    /// Intent region.
    /// </summary>
    SpeechServiceConnection_IntentRegion (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_IntentRegion),

    /// <summary>
    /// Intent source language. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_IntentSourceLanguage (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_IntentSourceLanguage),

    /// <summary>
    /// Recognition mode.
    /// </summary>
    SpeechServiceConnection_RecoMode (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_RecoMode),

    /// <summary>
    /// Interactive mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Interactive ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Interactive),

    /// <summary>
    /// Conversation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Conversation ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Conversation),

    /// <summary>
    /// Dictation mode. TODO:REMOVE
    /// </summary>
    SpeechServiceConnection_RecoMode_Dictation ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_RecoMode_Dictation),

    /// <summary>
    /// Recognition language.
    /// </summary>
    SpeechServiceConnection_RecoLanguage ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceConnection_RecoLanguage),

    /// <summary>
    /// Session id. TODO: REMOVE
    /// </summary>
    Speech_SessionId ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.Speech_SessionId),

    /// <summary>
    /// Detailed result required.
    /// </summary>
    SpeechServiceResponse_RequestDetailedResultTrueFalse (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse),

    /// <summary>
    /// Profanity filtering required.
    /// </summary>
    SpeechServiceResponse_RequestProfanityFilterTrueFalse (com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse),

    /// <summary>
    /// TODO:WHAT IS THIS FOR?
    /// </summary>
    SpeechServiceResponse_Json ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceResponse_Json),

    /// <summary>
    /// JSON in result.
    /// </summary>
    SpeechServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceResponse_JsonResult),

    /// <summary>
    /// Error details.
    /// </summary>
    SpeechServiceResponse_JsonErrorDetails ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.SpeechServiceResponse_JsonErrorDetails),

    /// <summary>
    /// Cancellation reason.
    /// </summary>
    CancellationDetails_ReasonCanceled ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.CancellationDetails_ReasonCanceled),

    /// <summary>
    /// Cancellation text.
    /// </summary>
    CancellationDetails_ReasonText ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.CancellationDetails_ReasonText),

    /// <summary>
    /// Cancellation detailed text.
    /// </summary>
    CancellationDetails_ReasonDetailedText ( com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId.CancellationDetails_ReasonDetailedText);

    private SpeechPropertyId(com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId id) {
        this.id = id;
    }

    /**
      * Returns the internal value speech property id
      *
      * @param 
      * @return the speech property id
      */
    public com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.SpeechPropertyId id;
}
