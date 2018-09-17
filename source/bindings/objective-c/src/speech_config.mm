//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SpeechConfig
{
    std::shared_ptr<SpeechImpl::SpeechConfig> speechConfigImpl;
}

- (instancetype)init:(std::shared_ptr<SpeechImpl::SpeechConfig>)handle
{
    self = [super init];
    speechConfigImpl = handle;
    return self;
}

- (std::shared_ptr<SpeechImpl::SpeechConfig>)getHandle
{
    return speechConfigImpl;
}

+ (SpeechConfig*) fromSubscription:(NSString *)subscriptionKey andRegion:(NSString *)region
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromSubscription([subscriptionKey string], [region string]);
        if (configImpl == nullptr)
            return nil;
        return [[SpeechConfig alloc] init :configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}


+ (SpeechConfig*)fromAuthorizationToken:(NSString *)authToken andRegion:(NSString *)region
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromAuthorizationToken([authToken string], [region string]);
        if (configImpl == nullptr)
            return nil;
        return [[SpeechConfig alloc] init :configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}


+ (SpeechConfig*)fromEndpoint:(NSString *)endpointUri andSubscription:(NSString *)subscriptionKey
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromEndpoint([endpointUri string], [subscriptionKey string]);
        if (configImpl == nullptr)
            return nil;
        return [[SpeechConfig alloc] init :configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (void)setSpeechRecognitionLanguage: (NSString *)lang
{
    speechConfigImpl->SetSpeechRecognitionLanguage([lang string]);
}

- (NSString *)getSpeechRecognitionLanguage
{
    return [NSString stringWithString:speechConfigImpl->GetSpeechRecognitionLanguage()];
}

- (void)setEndpointId: (NSString *)endpointId
{
    speechConfigImpl->SetEndpointId([endpointId string]);
}

- (NSString *)getEndpointId
{
    return [NSString stringWithString:speechConfigImpl->GetEndpointId()];
}

- (void)setAuthorizationToken: (NSString *)token
{
    speechConfigImpl->SetAuthorizationToken([token string]);
}

- (NSString *)getAuthorizationToken
{
    return [NSString stringWithString:speechConfigImpl->GetAuthorizationToken()];
}

- (NSString *)getSubscriptionKey
{
    return [NSString stringWithString:speechConfigImpl->GetSubscriptionKey()];
}

- (NSString *)getRegion
{
    return [NSString stringWithString:speechConfigImpl->GetRegion()];
}

- (void)setValue: (NSString *)value ToPropertyWithName: (NSString *)name
{
    return speechConfigImpl->SetProperty([name string], [value string]);
}

- (NSString *)getValueOfPropertyWithName: (NSString *)name
{
    return [NSString stringWithString:speechConfigImpl->GetProperty([name string])];
}

@end
