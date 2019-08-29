//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation PropertyCollection
{
    SpeechImpl::PropertyCollection *propertiesImpl;
}

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle
{
    static_assert((int)SPXSpeechServiceConnectionKey == (int)SpeechImpl::PropertyId::SpeechServiceConnection_Key, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionEndpoint == (int)SpeechImpl::PropertyId::SpeechServiceConnection_Endpoint, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionRegion == (int)SpeechImpl::PropertyId::SpeechServiceConnection_Region, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceAuthorizationToken == (int)SpeechImpl::PropertyId::SpeechServiceAuthorization_Token, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceAuthorizationType == (int)SpeechImpl::PropertyId::SpeechServiceAuthorization_Type, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionEndpointId == (int)SpeechImpl::PropertyId::SpeechServiceConnection_EndpointId, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionProxyHostName == (int)SpeechImpl::PropertyId::SpeechServiceConnection_ProxyHostName, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionProxyPort == (int)SpeechImpl::PropertyId::SpeechServiceConnection_ProxyPort, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionProxyUserName == (int)SpeechImpl::PropertyId::SpeechServiceConnection_ProxyUserName, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionProxyPassword == (int)SpeechImpl::PropertyId::SpeechServiceConnection_ProxyPassword, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionUrl == (int)SpeechImpl::PropertyId::SpeechServiceConnection_Url, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionTranslationToLanguages == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationToLanguages, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionTranslationVoice == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationVoice, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionTranslationFeatures == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationFeatures, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionIntentRegion == (int)SpeechImpl::PropertyId::SpeechServiceConnection_IntentRegion, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionRecognitionMode == (int)SpeechImpl::PropertyId::SpeechServiceConnection_RecoMode, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionRecognitionLanguage == (int)SpeechImpl::PropertyId::SpeechServiceConnection_RecoLanguage, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechSessionId == (int)SpeechImpl::PropertyId::Speech_SessionId, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionSynthesisLanguage == (int)SpeechImpl::PropertyId::SpeechServiceConnection_SynthLanguage, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionSynthesisVocie == (int)SpeechImpl::PropertyId::SpeechServiceConnection_SynthVoice, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionSynthesisOutputFormat == (int)SpeechImpl::PropertyId::SpeechServiceConnection_SynthOutputFormat, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionInitialSilenceTimeoutMs == (int)SpeechImpl::PropertyId::SpeechServiceConnection_InitialSilenceTimeoutMs, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionEndSilenceTimeoutMs == (int)SpeechImpl::PropertyId::SpeechServiceConnection_EndSilenceTimeoutMs, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionEnableAudioLogging == (int)SpeechImpl::PropertyId::SpeechServiceConnection_EnableAudioLogging, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRequestDetailedResultTrueFalse == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRequestProfanityFilterTrueFalse == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RequestProfanityFilterTrueFalse, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseProfanityOption == (int)SpeechImpl::PropertyId::SpeechServiceResponse_ProfanityOption, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponsePostProcessingOption == (int)SpeechImpl::PropertyId::SpeechServiceResponse_PostProcessingOption, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRequestWordLevelTimestamps == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RequestWordLevelTimestamps, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseStablePartialResultThreshold == (int)SpeechImpl::PropertyId::SpeechServiceResponse_StablePartialResultThreshold, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseOutputFormatOption == (int)SpeechImpl::PropertyId::SpeechServiceResponse_OutputFormatOption, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseTranslationRequestStablePartialResult == (int)SpeechImpl::PropertyId::SpeechServiceResponse_TranslationRequestStablePartialResult, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseJsonResult == (int)SpeechImpl::PropertyId::SpeechServiceResponse_JsonResult, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRecognitionLatencyMs == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RecognitionLatencyMs, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseJsonErrorDetails == (int)SpeechImpl::PropertyId::SpeechServiceResponse_JsonErrorDetails, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReason == (int)SpeechImpl::PropertyId::CancellationDetails_Reason, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReasonText == (int)SpeechImpl::PropertyId::CancellationDetails_ReasonText, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReasonDetailedText == (int)SpeechImpl::PropertyId::CancellationDetails_ReasonDetailedText, "inconsistent enum definition of property id");
    static_assert((int)SPXLanguageUnderstandingServiceResponseJsonResult == (int)SpeechImpl::PropertyId::LanguageUnderstandingServiceResponse_JsonResult, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechLogFilename == (int)SpeechImpl::PropertyId::Speech_LogFilename, "inconsistent enum definition of property id");

    self = [super init];
    propertiesImpl = propertiesHandle;
    return self;
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty([name toSpxString])];
}

-(NSString *)getPropertyByName:(NSString *)name defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty([name toSpxString], [defaultValue toSpxString])];
}

-(void)setPropertyTo:(NSString *)value byName:(NSString *)name
{
    propertiesImpl->SetProperty([name toSpxString], [value toSpxString]);
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId, [defaultValue toSpxString])];
}

-(void)setPropertyTo:(NSString *)value byId:(SPXPropertyId)propertyId
{
    propertiesImpl->SetProperty((SpeechImpl::PropertyId)(int)propertyId, [value toSpxString]);
}


@end

@implementation RecognizerPropertyCollection
{
    // We have to keep the recognizer instance alive in order to make sure the propertyHandle is valid
    RecognizerSharedPtr recognizerImpl;
}

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle
{
    self = [super initWithPropertyCollection:propertiesHandle];
    if (self)
        self->recognizerImpl = recoHandle;
    return self;
}

@end

@implementation SpeechSynthesizerPropertyCollection
{
    // We have to keep the speech synthesizer instance alive in order to make sure the propertyHandle is valid
    SpeechSynthSharedPtr speechSynthesizerImpl;
}

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(SpeechSynthSharedPtr)speechSynthHandle
{
    self = [super initWithPropertyCollection:propertiesHandle];
    if (self)
        self->speechSynthesizerImpl = speechSynthHandle;
    return self;
}

@end
