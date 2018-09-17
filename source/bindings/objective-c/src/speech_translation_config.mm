//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SpeechTranslationConfig
{
    std::shared_ptr<TranslationImpl::SpeechTranslationConfig> translationConfigImpl;
}

- (instancetype)init:(std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)handle
{
    self = [super init :handle];
    translationConfigImpl = handle;
    return self;
}

- (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)getHandle
{
    return translationConfigImpl;
}

+ (SpeechTranslationConfig *) fromSubscription:(NSString *)subscriptionKey andRegion:(NSString *)region
{
    try {
        auto impl = TranslationImpl::SpeechTranslationConfig::FromSubscription([subscriptionKey string], [region string]);
        if (impl == nullptr)
            return nil;
        return [[SpeechTranslationConfig alloc] init :impl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}


+ (SpeechTranslationConfig *)fromAuthorizationToken:(NSString *)authToken andRegion:(NSString *)region
{
    try {
        auto impl = TranslationImpl::SpeechTranslationConfig::FromAuthorizationToken([authToken string], [region string]);
        if (impl == nullptr)
            return nil;
        return [[SpeechTranslationConfig alloc] init :impl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}


+ (SpeechTranslationConfig *)fromEndpoint:(NSString *)endpointUri andSubscription:(NSString *)subscription
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromEndpoint([endpointUri string], [subscription string]);
        if (configImpl == nullptr)
            return nil;
        return [[SpeechTranslationConfig alloc] init :configImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (void)addTargetLanguage:(NSString *)lang
{
    translationConfigImpl->AddTargetLanguage([lang string]);
}

- (NSArray *)getTargetLanguages
{
    auto langsVector = translationConfigImpl->GetTargetLanguages();
    NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:langsVector.size()];
    for (std::vector<std::string>::iterator it = langsVector.begin(); it != langsVector.end(); ++it){
        [mutableArray addObject:[NSString stringWithString:*it]];
    }
    return mutableArray;
}

- (void)setVoiceName:(NSString *)voice
{
    translationConfigImpl->SetVoiceName([voice string]);
}

- (NSString *)getVoiceName
{
    return [NSString stringWithString:translationConfigImpl->GetVoiceName()];
}

@end
