//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>

#import "NSString_STL.h"
#import "speech_factory.h"
#import "speech_recognizer.h"
#import "speech_recognizer_private.h"
#import "speechapi_cxx.h"

@implementation SpeechFactory
{
    std::shared_ptr<Microsoft::CognitiveServices::Speech::ICognitiveServicesSpeechFactory> factoryImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscription AndRegion:(NSString *)region
{
    std::wstring subscriptionWString = [subscription wstring];
    std::wstring regionWString = [region wstring];
    
    try {
        factoryImpl = Microsoft::CognitiveServices::Speech::SpeechFactory::FromSubscription(subscriptionWString, regionWString);
        if (factoryImpl == nullptr)
            return nil;
        _subscriptionKey = subscription;
        _region = region;
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
    std::wstring endpointWString = [endpoint wstring];
    std::wstring subscriptionKeyWString = [subscriptionKey wstring];

    try {
        factoryImpl = Microsoft::CognitiveServices::Speech::SpeechFactory::FromEndpoint(endpointWString, subscriptionKeyWString);
        if (factoryImpl == nullptr)
            return nil;
        _subscriptionKey = subscriptionKey;
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

- (SpeechRecognizer*)createSpeechRecognizerWithDefaultMicrophone
{
    try {
        std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> recoImpl = factoryImpl->CreateSpeechRecognizer();
        if (recoImpl == nullptr)
            return nil;
        SpeechRecognizer *reco = [[SpeechRecognizer alloc] init:(void *)&recoImpl];
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
    std::wstring pathWString = [path wstring];
    
    try
    {
        std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> recoImpl = factoryImpl->CreateSpeechRecognizerWithFileInput(pathWString);
        if (recoImpl == nullptr)
            return nil;
        SpeechRecognizer *reco = [[SpeechRecognizer alloc] init:(void *)&recoImpl];
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

@end
