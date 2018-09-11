//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speech_factory.h"
#import "speech_recognizer_private.h"
#import "translation_recognizer_private.h"
#import "intent_recognizer_private.h"
#import "common_private.h"

@implementation SpeechFactory
{
    std::shared_ptr<SpeechImpl::ICognitiveServicesSpeechFactory> factoryImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscription AndRegion:(NSString *)region
{
    std::string subscriptionString = [subscription string];
    std::string regionString = [region string];
    
    try {
        factoryImpl = SpeechImpl::SpeechFactory::FromSubscription(subscriptionString, regionString);
        if (factoryImpl == nullptr)
            return nil;
        return self;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    
    return nil;
}

- (instancetype)initWithAuthToken:(NSString *)token AndRegion:(NSString *)region
{
    std::string tokenString = [token string];
    std::string regionString = [region string];
    
    try {
        factoryImpl = SpeechImpl::SpeechFactory::FromAuthorizationToken(tokenString, regionString);
        if (factoryImpl == nullptr)
            return nil;
        return self;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    
    return nil;
}

- (instancetype)initWithEndpoint:(NSString *)endpoint AndSubscription:(NSString *)subscriptionKey
{
    std::string endpointString = [endpoint string];
    std::string subscriptionKeyString = [subscriptionKey string];

    try {
        factoryImpl = SpeechImpl::SpeechFactory::FromEndpoint(endpointString, subscriptionKeyString);
        if (factoryImpl == nullptr)
            return nil;
        return self;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }

    return nil;
}

- (void)dealloc
{
    if (factoryImpl != nullptr) {
        factoryImpl.reset();
    }
}

- (SpeechRecognizer*)createSpeechRecognizer
{
    try {
        SpeechRecoSharedPtr recoImpl = factoryImpl->CreateSpeechRecognizer();
        if (recoImpl == nullptr)
            return nil;
        SpeechRecognizer *reco = [[SpeechRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (SpeechRecognizer*)createSpeechRecognizerWithFileInput:(NSString *)path
{
    std::string pathString = [path string];
    try
    {
        SpeechRecoSharedPtr recoImpl = factoryImpl->CreateSpeechRecognizerWithFileInputHACKFOROBJECTIVEC(pathString);
        if (recoImpl == nullptr)
            return nil;
        SpeechRecognizer *reco = [[SpeechRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...)
    {
        // If the exception happens on 
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (IntentRecognizer*)createIntentRecognizer
{
    try {
        IntentRecoSharedPtr recoImpl = factoryImpl->CreateIntentRecognizer();
        if (recoImpl == nullptr)
            return nil;
        IntentRecognizer *reco = [[IntentRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (IntentRecognizer*)createIntentRecognizerWithFileInput:(NSString *)path
{
    std::string pathString = [path string];
    try
    {
        IntentRecoSharedPtr recoImpl = factoryImpl->CreateIntentRecognizerWithFileInputHACKFOROBJECTIVEC(pathString);
        if (recoImpl == nullptr)
            return nil;
        IntentRecognizer *reco = [[IntentRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...)
    {
        // If the exception happens on 
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (TranslationRecognizer*)createTranslationRecognizerFromLanguage:(NSString *)from ToLanguages:(NSArray *)to
{
    std::vector<std::string> toList;
    NSEnumerator *it = [to objectEnumerator];
    NSString* lang;
    while (lang = [it nextObject]) {
        toList.push_back([lang string]);
    }

    try {
        TranslationRecoSharedPtr recoImpl = factoryImpl->CreateTranslationRecognizerFromConfig([from string], toList);
        if (recoImpl == nullptr)
            return nil;
        TranslationRecognizer *reco = [[TranslationRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (TranslationRecognizer*)createTranslationRecognizerFromLanguage:(NSString *)from ToLanguages:(NSArray *)to WithVoiceOutput:(NSString *)voice
{
    std::vector<std::string> toList;
    NSEnumerator *it = [to objectEnumerator];
    NSString* lang;
    while (lang = [it nextObject]) {
        toList.push_back([lang string]);
    }

    try {
        TranslationRecoSharedPtr recoImpl = factoryImpl->CreateTranslationRecognizerFromConfig([from string], toList, [voice string]);
        if (recoImpl == nullptr)
            return nil;
        TranslationRecognizer *reco = [[TranslationRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (TranslationRecognizer*)createTranslationRecognizerWithFileInput:(NSString *)path FromLanguage:(NSString *)from ToLanguages:(NSArray *)to
{
    std::vector<std::string> toList;
    NSEnumerator *it = [to objectEnumerator];
    NSString* lang;
    while (lang = [it nextObject]) {
        toList.push_back([lang string]);
    }

    try {
        TranslationRecoSharedPtr recoImpl = factoryImpl->CreateTranslationRecognizerWithFileInputHACKFOROBJECTIVEC([from string], toList,[path string]);
        if (recoImpl == nullptr)
            return nil;
        TranslationRecognizer *reco = [[TranslationRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

- (TranslationRecognizer*)createTranslationRecognizerWithFileInput:(NSString *)path FromLanguage:(NSString *)from ToLanguages:(NSArray *)to WithVoiceOutput:(NSString *)voice
{
    std::vector<std::string> toList;
    NSEnumerator *it = [to objectEnumerator];
    NSString* lang;
    while (lang = [it nextObject]) {
        toList.push_back([lang string]);
    }

    try {
        TranslationRecoSharedPtr recoImpl = factoryImpl->CreateTranslationRecognizerWithFileInputHACKFOROBJECTIVEC([from string], toList, [voice string], [path string]);
        if (recoImpl == nullptr)
            return nil;
        TranslationRecognizer *reco = [[TranslationRecognizer alloc] init :recoImpl];
        return reco;
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught.");
    }
    return nil;
}

+ (SpeechFactory*) fromSubscription:(NSString *)subscription AndRegion:(NSString *)region
{
    SpeechFactory *factory = [[SpeechFactory alloc] initWithSubscription:subscription AndRegion:region];
    return factory;
}

+ (SpeechFactory*)fromEndpoint:(NSString *)endpoint AndSubscription:(NSString *)subscription
{
    SpeechFactory *factory = [[SpeechFactory alloc] initWithEndpoint:endpoint AndSubscription:subscription];
    return factory;
}

+ (SpeechFactory*)fromAuthorizationToken:(NSString *)authToken AndRegion:(NSString *)region
{
    SpeechFactory *factory = [[SpeechFactory alloc] initWithAuthToken:authToken AndRegion:region];
    return factory;
}

@end
