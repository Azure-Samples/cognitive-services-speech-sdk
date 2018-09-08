//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognizer_private.h"
#import "speech_recognizer_private.h"
#import "speech_recognition_result_private.h"
#import "speech_recognition_result_event_args_private.h"
#import "session_event_args_private.h"
#import "recognition_event_args_private.h"
#import "common_private.h"

namespace SpeechImpl = Microsoft::CognitiveServices::Speech;

struct EventHandlerHelper
{
    SpeechRecognizer *recognizer;
    SpeechRecoSharedPtr recoImpl;

    EventHandlerHelper(SpeechRecognizer *reco, SpeechRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addFinalResultEventHandler()
    {
        NSLog(@"Add FinalResultEventHandler");
        recoImpl->FinalResult.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionResultEventArgs *eventArgs = [[SpeechRecognitionResultEventArgs alloc] init: e];
                [recognizer onFinalResultEvent: eventArgs];
            });
    }
    
    void addIntermediateResultEventHandler()
    {
        NSLog(@"Add IntermediateResultEventHandler");
        recoImpl->IntermediateResult.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionResultEventArgs *eventArgs = [[SpeechRecognitionResultEventArgs alloc] init: e];
                [recognizer onIntermediateResultEvent: eventArgs];
            });
    }
    
    void addSessionEventHandler()
    {
        NSLog(@"Add SessionEventHandler");
        recoImpl->SessionStarted.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] initWithHandle :SessionStartedEvent :e];
                [recognizer onSessionEvent: eventArgs];
            });
        
        recoImpl->SessionStopped.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] initWithHandle :SessionStoppedEvent :e];
                [recognizer onSessionEvent: eventArgs];
            });
    }
    
    void addRecognitionEventHandler()
    {
        NSLog(@"Add RecognitionEventHandler");
        recoImpl->SpeechStartDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] initWithHandle :SpeechStartDetectedEvent :e];
                [recognizer onRecognitionEvent: eventArgs];
            });
        
        recoImpl->SpeechEndDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] initWithHandle :SpeechEndDetectedEvent : e];
                [recognizer onRecognitionEvent: eventArgs];
            });
    }
};

@implementation SpeechRecognizer
{
    SpeechRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;
    NSMutableArray *finalResultEventListenerList;
    NSMutableArray *intermediateResultEventListenerList;
    struct EventHandlerHelper *eventImpl;
    NSLock *arrayLock;
}

- (instancetype)init :(SpeechRecoSharedPtr)recoHandle
{
    self = [super init];
    recoImpl = recoHandle;
    if (recoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        
        finalResultEventListenerList = [NSMutableArray array];
        intermediateResultEventListenerList = [NSMutableArray array];
        arrayLock = [[NSLock alloc] init];
        
        eventImpl = new EventHandlerHelper(self, recoImpl);
        [super setDispatchQueue: dispatchQueue];
        eventImpl->addIntermediateResultEventHandler();
        eventImpl->addFinalResultEventHandler();
        eventImpl->addSessionEventHandler();
        eventImpl->addRecognitionEventHandler();
        
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
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: resultImpl];
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
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithRuntimeError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: resultImpl];
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

- (void)stopContinuousRecognition
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
    NSLog(@"OBJC: onFinalResultEvent");
    [arrayLock lock];
    for (id handle in finalResultEventListenerList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionResultEventHandlerBlock)handle)(self, eventArgs);
        });
    }
    [arrayLock unlock];
}

- (void)onIntermediateResultEvent:(SpeechRecognitionResultEventArgs *)eventArgs
{
    NSLog(@"OBJC: onIntermediateResultEvent");
    [arrayLock lock];
    for (id handle in intermediateResultEventListenerList) {
        dispatch_async(dispatchQueue, ^{
            ((RecognitionResultEventHandlerBlock)handle)(self, eventArgs);
        });
    }
    [arrayLock unlock];
}

- (void)addFinalResultEventListener:(RecognitionResultEventHandlerBlock)eventHandler
{
    [arrayLock lock];
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
