//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXSpeechConfiguration
{
    std::shared_ptr<SpeechImpl::SpeechConfig> speechConfigImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscriptionKey region:(NSString *)region
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromSubscription([subscriptionKey string], [region string]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (instancetype)initWithAuthorizationToken:(NSString *)authToken region:(NSString *)region
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromAuthorizationToken([authToken string], [region string]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (instancetype)initWithEndpoint:(NSString *)endpointUri subscription:(NSString *)subscriptionKey
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromEndpoint([endpointUri string], [subscriptionKey string]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SpeechConfig>)configImpl
{
    self = [super init];
    if (!self || configImpl == nullptr) {
        return nil;
    }
    speechConfigImpl = configImpl;
    return self;
}

- (std::shared_ptr<SpeechImpl::SpeechConfig>)getHandle
{
    return speechConfigImpl;
}

- (void)setSpeechRecognitionLanguage: (NSString *)lang
{
    speechConfigImpl->SetSpeechRecognitionLanguage([lang string]);
}

- (NSString *)speechRecognitionLanguage
{
    return [NSString StringWithStdString:speechConfigImpl->GetSpeechRecognitionLanguage()];
}

- (void)setEndpointId: (NSString *)endpointId
{
    speechConfigImpl->SetEndpointId([endpointId string]);
}

- (NSString *)endpointId
{
    return [NSString StringWithStdString:speechConfigImpl->GetEndpointId()];
}

- (void)setAuthorizationToken: (NSString *)token
{
    speechConfigImpl->SetAuthorizationToken([token string]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:speechConfigImpl->GetAuthorizationToken()];
}

- (NSString *)subscriptionKey
{
    return [NSString StringWithStdString:speechConfigImpl->GetSubscriptionKey()];
}

- (NSString *)region
{
    return [NSString StringWithStdString:speechConfigImpl->GetRegion()];
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:speechConfigImpl->GetProperty([name string])];
}

-(NSString *)setPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:speechConfigImpl->GetProperty([name string])];
}

-(void)setPropertyTo:(NSString *)value byName:(NSString *)name
{
    speechConfigImpl->SetProperty([name string], [value string]);
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString StringWithStdString:speechConfigImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(void)setPropertyTo:(NSString *)value byId:(SPXPropertyId)propertyId
{
    speechConfigImpl->SetProperty((SpeechImpl::PropertyId)(int)propertyId, [value string]);
}

@end
