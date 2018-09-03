//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speech_recognizer.h"
#import "speech_recognizer_private.h"
#import "speech_recognition_result_private.h"
#import "speech_recognition_result_event_args_private.h"
#import "speechapi_cxx.h"

struct EventHandlerHelper
{
    SpeechRecognizer *recognizer;

    EventHandlerHelper(SpeechRecognizer *reco) : recognizer(reco)
    {
    }
    
    void addFinalResultEventHandler()
    {
        void *handle = [recognizer getRecoHandle];
        auto recoImpl = *static_cast<std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> *>(handle);
        recoImpl->FinalResult.Connect([this] (const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionResultEventArgs *eventArgs = [[SpeechRecognitionResultEventArgs alloc] init: (void *)&e];
                [recognizer onFinalResultEvent: eventArgs];
            });
    }
    
    void addIntermediateResultEventHandler()
    {
        void *handle = [recognizer getRecoHandle];
        auto recoImpl = *static_cast<std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> *>(handle);
        recoImpl->IntermediateResult.Connect([this] (const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionResultEventArgs *eventArgs = [[SpeechRecognitionResultEventArgs alloc] init: (void *)&e];
                [recognizer onIntermediateResultEvent: eventArgs];
            });
    }
};

@implementation SpeechRecognizer
{
    std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> recoImpl;
    dispatch_queue_t dispatchQueue;
    NSMutableArray *finalResultEventListenerList;
    NSMutableArray *intermediateResultEventListenerList;
    struct EventHandlerHelper *eventImpl;
    NSLock *arrayLock;
}

- (instancetype)init:(void *)recoHandle
{
    self = [super init];
    recoImpl = *static_cast<std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> *>(recoHandle);
    if (recoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        finalResultEventListenerList = [NSMutableArray array];
        intermediateResultEventListenerList = [NSMutableArray array];
        arrayLock = [[NSLock alloc] init];
        eventImpl = new EventHandlerHelper(self);
        return self;
    }
}

- (void)dealloc
{
    [self close];
    delete eventImpl;
}

- (SpeechRecognitionResult *)recognize
{
    SpeechRecognitionResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: (void *)&resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeAsync:(void (^)(SpeechRecognitionResult *))resultReceivedBlock
{
    SpeechRecognitionResult *result = nil;
    if (recoImpl == nullptr) {
        result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"Recognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedBlock(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: (void *)&resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"Runtime Exception"];
    }
    
    dispatch_async(dispatchQueue, ^{
        resultReceivedBlock(result);
    });
}

- (void)startContinuousRecognition
{
    if (recoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"Recognizer handle is null");
        return;
    }
    
    try {
        recoImpl->StartContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)stopContinuousRecognition;
{
    if (recoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"Recognizer handle is null");
        return;
    }
    
    try {
        recoImpl->StopContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)close
{
    if (recoImpl != nullptr) {
        recoImpl.reset();
    }
}
- (void *)getRecoHandle
{
    return (void *)&recoImpl;
}

- (void)onFinalResultEvent:(SpeechRecognitionResultEventArgs *)eventArgs
{
    [arrayLock lock];
    for (id handle in finalResultEventListenerList) {
        ((RecognitionResultEventHandlerBlock)handle)(self, eventArgs);
    }
    [arrayLock unlock];
}

- (void)onIntermediateResultEvent:(SpeechRecognitionResultEventArgs *)eventArgs
{
    [arrayLock lock];
    for (id handle in intermediateResultEventListenerList) {
        ((RecognitionResultEventHandlerBlock)handle)(self, eventArgs);
    }
    [arrayLock unlock];
}

- (void)addFinalResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    if ([finalResultEventListenerList count] == 0)
        eventImpl->addFinalResultEventHandler();
    [finalResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}

- (void)removeFinalResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    // Todo: disconnect at C++
    [finalResultEventListenerList removeObject:eventHandler];
    [arrayLock unlock];
    return;
}

- (void)addIntermediateResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    if ([intermediateResultEventListenerList count] == 0)
        eventImpl->addIntermediateResultEventHandler();
    [intermediateResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}

- (void)removeIntermediateResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    // Todo: disconnect at C++
    [intermediateResultEventListenerList removeObject:eventHandler];
    [arrayLock unlock];
    return;
}

@end
