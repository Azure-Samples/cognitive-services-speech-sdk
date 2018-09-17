//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct SpeechEventHandlerHelper
{
    SpeechRecognizer *recognizer;
    SpeechRecoSharedPtr recoImpl;

    SpeechEventHandlerHelper(SpeechRecognizer *reco, SpeechRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        NSLog(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionEventArgs *eventArgs = [[SpeechRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizedEvent: eventArgs];
            });
    }
    
    void addRecognizingEventHandler()
    {
        NSLog(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SpeechRecognitionEventArgs *eventArgs = [[SpeechRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizingEvent: eventArgs];
            });
    }

    void addCanceledEventHandler()
    {
        NSLog(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const SpeechImpl::SpeechRecognitionCanceledEventArgs& e)
            {
                SpeechRecognitionCanceledEventArgs *eventArgs = [[SpeechRecognitionCanceledEventArgs alloc] init:e];
                [recognizer onCanceledEvent: eventArgs];
            });
    }

    void addSessionStartedEventHandler()
    {
        NSLog(@"Add SessionStartedEventHandler");
        recoImpl->SessionStarted.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] init:e];
                [recognizer onSessionStartedEvent: eventArgs];
            });
    }
    
    void addSessionStoppedEventHandler()
    {
        NSLog(@"Add SessionStoppedEventHandler");
        recoImpl->SessionStopped.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] init:e];
                [recognizer onSessionStoppedEvent: eventArgs];
            });
    }
    
    void addSpeechStartDetectedEventHandler()
    {
        NSLog(@"Add SpeechStartDetectedEventHandler");
        recoImpl->SpeechStartDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] init:e];
                [recognizer onSpeechStartDetectedEvent: eventArgs];
            });
    }
    
    void addSpeechEndDetectedEventHandler()
    {
        NSLog(@"Add SpeechStopDetectedEventHandler");
        recoImpl->SpeechEndDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] init:e];
                [recognizer onSpeechEndDetectedEvent: eventArgs];
            });
    }
};

@implementation SpeechRecognizer
{
    SpeechRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;
    
    NSMutableArray *recognizedEventListenerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventListenerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventListenerList;
    NSLock *canceledLock;
    struct SpeechEventHandlerHelper *eventImpl;
    
    RecognizerPropertyCollection *propertyCollection;
}

+ (SpeechRecognizer *)fromConfig: (SpeechConfig *)speechConfig
{
    try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfig getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [[SpeechRecognizer alloc] init :recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SpeechRecognizer in core.");
    }
    return nil;
}

+ (SpeechRecognizer *)fromConfig: (SpeechConfig *)speechConfig usingAudio: (AudioConfig *)audioConfig
{
    try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfig getHandle], [audioConfig getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [[SpeechRecognizer alloc] init :recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SpeechRecognizer in core.");
    }
    return nil;
}


- (instancetype)init :(SpeechRecoSharedPtr)recoHandle
{
    self = [super initFrom:recoHandle withParameters:&recoHandle->Properties];
    recoImpl = recoHandle;
    if (recoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        recognizedEventListenerList = [NSMutableArray array];
        recognizingEventListenerList = [NSMutableArray array];
        canceledEventListenerList = [NSMutableArray array];
        recognizedLock = [[NSLock alloc] init];
        recognizingLock = [[NSLock alloc] init];
        canceledLock = [[NSLock alloc] init];
        
        eventImpl = new SpeechEventHandlerHelper(self, recoImpl);
        [super setDispatchQueue: dispatchQueue];
        eventImpl->addRecognizingEventHandler();
        eventImpl->addRecognizedEventHandler();
        eventImpl->addCanceledEventHandler();
        eventImpl->addSessionStartedEventHandler();
        eventImpl->addSessionStoppedEventHandler();
        eventImpl->addSpeechStartDetectedEventHandler();
        eventImpl->addSpeechEndDetectedEventHandler();

        return self;
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    recoImpl->SetAuthorizationToken([token string]);
}

- (NSString *)getAuthorizationToken
{
    return [NSString stringWithString:recoImpl->GetAuthorizationToken()];
}

- (NSString *)getEndpointId
{
    return [NSString stringWithString:recoImpl->GetEndpointId()];
}

- (SpeechRecognitionResult *)recognizeOnce
{
    SpeechRecognitionResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[SpeechRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(SpeechRecognitionResult *))resultReceivedBlock
{
    SpeechRecognitionResult *result = nil;
    if (recoImpl == nullptr) {
        result = [[SpeechRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedBlock(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SpeechRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SpeechRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
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

- (void)onRecognizedEvent:(SpeechRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventListenerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SpeechRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(SpeechRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventListenerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SpeechRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(SpeechRecognitionCanceledEventArgs *)eventArgs
{
    NSLog(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventListenerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SpeechRecognitionCanceledEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler
{
    [recognizedLock lock];
    [recognizedEventListenerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventListener:(SpeechRecognitionEventHandlerBlock)eventHandler
{
    [recognizingLock lock];
    [recognizingEventListenerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventListener:(SpeechRecognitionCanceledEventHandlerBlock)eventHandler
{
    [canceledLock lock];
    [canceledEventListenerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

@end
