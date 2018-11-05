//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines property ids.
 * Changed in version 1.1.0.
 */
public enum PropertyId {

    /**
     * Subscription key.
     */
    SpeechServiceConnection_Key (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Key),

    /**
     * Endpoint.
     */
    SpeechServiceConnection_Endpoint (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Endpoint),

    /**
     * Region.
     */
    SpeechServiceConnection_Region (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_Region),

    /**
     * Authorization token.
     */
    SpeechServiceAuthorization_Token (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Token),

    /**
     * Authorization type.
     */
    SpeechServiceAuthorization_Type (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceAuthorization_Type),

    /**
     * Endpoint ID.
     */
    SpeechServiceConnection_EndpointId (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_EndpointId),

    /**
     * The host name of the proxy server.
     * Added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyHostName (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyHostName),

    /**
     * The port of the proxy server.
     * Added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPort (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyPort),

    /**
     * The user name of the proxy server.
     * Added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyUserName (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyUserName),

    /**
     * The password of the proxy server.
     * Added in version 1.1.0.
     */
    SpeechServiceConnection_ProxyPassword (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_ProxyPassword),

    /**
     * Translation to languages.
     */
    SpeechServiceConnection_TranslationToLanguages (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationToLanguages),

    /**
     * Translation output voice.
     */
    SpeechServiceConnection_TranslationVoice (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationVoice),

    /**
     * Translation features.
     */
    SpeechServiceConnection_TranslationFeatures (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_TranslationFeatures),

    /**
     * Intent region.
     */
    SpeechServiceConnection_IntentRegion (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_IntentRegion),

    /**
     * Recognition mode. Can be "INTERACTIVE", "CONVERSATION", "DICTATION".
     */
    SpeechServiceConnection_RecoMode (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoMode),

    /**
     * Recognition language.
     */
    SpeechServiceConnection_RecoLanguage ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceConnection_RecoLanguage),

    /**
     * Session ID.
     */
    Speech_SessionId ( com.microsoft.cognitiveservices.speech.internal.PropertyId.Speech_SessionId),

    /**
     * Detailed result required.
     */
    SpeechServiceResponse_RequestDetailedResultTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse),

    /**
     * Profanity filtering required.
     */
    SpeechServiceResponse_RequestProfanityFilterTrueFalse (com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse),

    /**
     * JSON result of speech recognition service.
     */
    SpeechServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonResult),

    /**
     * Error details.
     */
    SpeechServiceResponse_JsonErrorDetails ( com.microsoft.cognitiveservices.speech.internal.PropertyId.SpeechServiceResponse_JsonErrorDetails),

    /**
     * Cancellation reason.
     */
    CancellationDetails_Reason ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_Reason),

    /**
     * Cancellation text.
     */
    CancellationDetails_ReasonText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonText),

    /**
     * Cancellation detailed text.
     */
    CancellationDetails_ReasonDetailedText ( com.microsoft.cognitiveservices.speech.internal.PropertyId.CancellationDetails_ReasonDetailedText),

    /**
     * JSON result of language understanding service.
     */
    LanguageUnderstandingServiceResponse_JsonResult ( com.microsoft.cognitiveservices.speech.internal.PropertyId.LanguageUnderstandingServiceResponse_JsonResult);

    private PropertyId(com.microsoft.cognitiveservices.speech.internal.PropertyId id) {
        this.id = id;
    }

    /**
     * Returns the internal value property id
     *
     * @return the speech property id
     */
    public com.microsoft.cognitiveservices.speech.internal.PropertyId getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.PropertyId id;
}
