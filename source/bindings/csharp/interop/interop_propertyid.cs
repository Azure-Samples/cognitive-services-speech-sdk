//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    internal enum PropertyId
    {
        SpeechServiceConnection_Key = 1000,
        SpeechServiceConnection_Endpoint = 1001,
        SpeechServiceConnection_Region = 1002,
        SpeechServiceAuthorization_Token = 1003,
        SpeechServiceAuthorization_Type = 1004,
        SpeechServiceConnection_EndpointId = 1005,
        SpeechServiceConnection_ProxyHostName = 1100,
        SpeechServiceConnection_ProxyPort = 1101,
        SpeechServiceConnection_ProxyUserName = 1102,
        SpeechServiceConnection_ProxyPassword = 1103,
        SpeechServiceConnection_TranslationToLanguages = 2000,
        SpeechServiceConnection_TranslationVoice = 2001,
        SpeechServiceConnection_TranslationFeatures = 2002,
        SpeechServiceConnection_IntentRegion = 2003,
        SpeechServiceConnection_RecoMode = 3000,
        SpeechServiceConnection_RecoLanguage = 3001,
        Speech_SessionId = 3002,
        SpeechServiceResponse_RequestDetailedResultTrueFalse = 4000,
        SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4001,
        SpeechServiceResponse_JsonResult = 5000,
        SpeechServiceResponse_JsonErrorDetails = 5001,
        SpeechServiceResponse_RecognitionLatencyMs = 5002,
        CancellationDetails_Reason = 6000,
        CancellationDetails_ReasonText = 6001,
        CancellationDetails_ReasonDetailedText = 6002,
        LanguageUnderstandingServiceResponse_JsonResult = 7000
    }
}
