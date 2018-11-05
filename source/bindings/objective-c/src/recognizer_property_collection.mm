//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation RecognizerPropertyCollection
{
    // We have to keep the recognizer instance alive in order to make sure the propertyHandle is valid
    RecognizerSharedPtr recognizerImpl;
    SpeechImpl::PropertyCollection *propertiesImpl;
}

-(instancetype)initWithPropertyCollection :(SpeechImpl::PropertyCollection *)propertiesHandle from:(RecognizerSharedPtr)recoHandle
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
    static_assert((int)SPXSpeechServiceConnectionTranslationToLanguages == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationToLanguages, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionTranslationVoice == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationVoice, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionTranslationFeatures == (int)SpeechImpl::PropertyId::SpeechServiceConnection_TranslationFeatures, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionIntentRegion == (int)SpeechImpl::PropertyId::SpeechServiceConnection_IntentRegion, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionRecognitionMode == (int)SpeechImpl::PropertyId::SpeechServiceConnection_RecoMode, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceConnectionRecognitionLanguage == (int)SpeechImpl::PropertyId::SpeechServiceConnection_RecoLanguage, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechSessionId == (int)SpeechImpl::PropertyId::Speech_SessionId, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRequestDetailedResultTrueFalse == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RequestDetailedResultTrueFalse, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseRequestProfanityFilterTrueFalse == (int)SpeechImpl::PropertyId::SpeechServiceResponse_RequestProfanityFilterTrueFalse, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseJsonResult == (int)SpeechImpl::PropertyId::SpeechServiceResponse_JsonResult, "inconsistent enum definition of property id");
    static_assert((int)SPXSpeechServiceResponseJsonErrorDetails == (int)SpeechImpl::PropertyId::SpeechServiceResponse_JsonErrorDetails, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReason == (int)SpeechImpl::PropertyId::CancellationDetails_Reason, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReasonText == (int)SpeechImpl::PropertyId::CancellationDetails_ReasonText, "inconsistent enum definition of property id");
    static_assert((int)SPXCancellationDetailsReasonDetailedText == (int)SpeechImpl::PropertyId::CancellationDetails_ReasonDetailedText, "inconsistent enum definition of property id");
    static_assert((int)SPXLanguageUnderstandingServiceResponseJsonResult == (int)SpeechImpl::PropertyId::LanguageUnderstandingServiceResponse_JsonResult, "inconsistent enum definition of property id");

    self = [super init];
    recognizerImpl = recoHandle;
    propertiesImpl = propertiesHandle;
    return self;
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty([name string])];
}

-(NSString *)getPropertyByName:(NSString *)name defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty([name string], [defaultValue string])];
}

-(void)setPropertyTo:(NSString *)value byName:(NSString *)name
{
    propertiesImpl->SetProperty([name string], [value string]);
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId defaultValue:(NSString *)defaultValue
{
    return [NSString StringWithStdString:propertiesImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId, [defaultValue string])];
}

-(void)setPropertyTo:(NSString *)value byId:(SPXPropertyId)propertyId
{
    propertiesImpl->SetProperty((SpeechImpl::PropertyId)(int)propertyId, [value string]);
}


@end
