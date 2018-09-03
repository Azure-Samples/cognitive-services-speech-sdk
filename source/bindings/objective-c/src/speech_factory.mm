//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>

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
    NSData *data = [subscription dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    std::wstring subscriptionWString = std::wstring((wchar_t *)[data bytes], [data length]/sizeof(wchar_t));
    NSData *data2 = [region dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    std::wstring regionWString = std::wstring((wchar_t *)[data2 bytes], [data2 length]/sizeof(wchar_t));
    
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
    NSData *data = [path dataUsingEncoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    std::wstring pathWString = std::wstring((wchar_t *)[data bytes], [data length]/sizeof(wchar_t));
    
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

@end
