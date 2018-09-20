//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXSpeechTranslationConfiguration
{
    std::shared_ptr<TranslationImpl::SpeechTranslationConfig> speechTranslationConfigurationImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscriptionKey region:(NSString *)region
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromSubscription([subscriptionKey string], [region string]);
        if (configImpl == nullptr) {
            return nil;
        }
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
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromAuthorizationToken([authToken string], [region string]);
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
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromEndpoint([endpointUri string], [subscriptionKey string]);
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

- (instancetype)initWithImpl:(std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)translationConfigurationImpl
{
    self = [super initWithImpl:translationConfigurationImpl];
    if (!self || translationConfigurationImpl == nullptr) {
        return nil;
    }
    self->speechTranslationConfigurationImpl = translationConfigurationImpl;
    return self;
}

- (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)getHandle
{
    return speechTranslationConfigurationImpl;
}

- (void)addTargetLanguage:(NSString *)lang
{
    speechTranslationConfigurationImpl->AddTargetLanguage([lang string]);
}

- (NSArray *)targetLanguages
{
    auto langsVector = speechTranslationConfigurationImpl->GetTargetLanguages();
    NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:langsVector.size()];
    for (std::vector<std::string>::iterator it = langsVector.begin(); it != langsVector.end(); ++it){
        [mutableArray addObject:[NSString stringWithString:*it]];
    }
    return mutableArray;
}

- (void)setVoiceName:(NSString *)voice
{
    speechTranslationConfigurationImpl->SetVoiceName([voice string]);
}

- (NSString *)voiceName
{
    return [NSString stringWithString:speechTranslationConfigurationImpl->GetVoiceName()];
}

@end
