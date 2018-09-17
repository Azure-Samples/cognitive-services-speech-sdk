//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct IntentEventHandlerHelper
{
    IntentRecognizer *recognizer;
    IntentRecoSharedPtr recoImpl;
    
    IntentEventHandlerHelper(IntentRecognizer *reco, IntentRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        NSLog(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
                                     {
                                         IntentRecognitionEventArgs *eventArgs = [[IntentRecognitionEventArgs alloc] init: e];
                                         [recognizer onRecognizedEvent: eventArgs];
                                     });
    }
    
    void addRecognizingEventHandler()
    {
        NSLog(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
                                      {
                                          IntentRecognitionEventArgs *eventArgs = [[IntentRecognitionEventArgs alloc] init: e];
                                          [recognizer onRecognizingEvent: eventArgs];
                                      });
    }
    
    void addCanceledEventHandler()
    {
        NSLog(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const IntentImpl::IntentRecognitionCanceledEventArgs& e)
                                   {
                                       IntentRecognitionCanceledEventArgs *eventArgs = [[IntentRecognitionCanceledEventArgs alloc] init:e];
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

@implementation IntentRecognizer
{
    IntentRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;
    
    NSMutableArray *recognizedEventListenerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventListenerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventListenerList;
    NSLock *canceledLock;
    
    struct IntentEventHandlerHelper *eventImpl;
}

+ (IntentRecognizer *)fromConfig: (SpeechConfig *)speechConfig
{
    try {
        auto intentRecoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfig getHandle]);
        if (intentRecoImpl == nullptr)
            return nil;
        return [[IntentRecognizer alloc] init :intentRecoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating IntentRecognizer in core.");
    }
    return nil;
}

+ (IntentRecognizer *)fromConfig: (SpeechConfig *)speechConfig usingAudio: (AudioConfig *)audioConfig
{
    try {
        auto intentRecoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfig getHandle], [audioConfig getHandle]);
        if (intentRecoImpl == nullptr)
            return nil;
        return [[IntentRecognizer alloc] init :intentRecoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating IntentRecognizer in core.");
    }
    return nil;
}


- (instancetype)init :(IntentRecoSharedPtr)recoHandle
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
        
        eventImpl = new IntentEventHandlerHelper(self, recoImpl);
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

- (void)addIntentFromPhrase:(NSString *)simplePhrase
{
    recoImpl->AddIntent([simplePhrase string]);
}

- (void)addIntentFromPhrase:(NSString *)simplePhrase mappingToId:(NSString *)intentId
{
    recoImpl->AddIntent([simplePhrase string], [intentId string]);
}

- (void)addIntent: (NSString *)intentName fromModel:(LanguageUnderstandingModel *)model
{
    recoImpl->AddIntent([model getModelHandle], [intentName string]);
}

- (void)addIntent: (NSString *)intentName fromModel:(LanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    recoImpl->AddIntent([model getModelHandle], [intentName string], [intentId string]);
}

- (void)addAllIntentsFromModel:(LanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    recoImpl->AddAllIntents([model getModelHandle], [intentId string]);
}

- (IntentRecognitionResult *)recognizeOnce
{
    IntentRecognitionResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[IntentRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[IntentRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[IntentRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[IntentRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(IntentRecognitionResult *))resultReceivedBlock
{
    IntentRecognitionResult *result = nil;
    if (recoImpl == nullptr) {
        result = [[IntentRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedBlock(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[IntentRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[IntentRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[IntentRecognitionResult alloc] initWithError: @"Runtime Exception"];
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

- (void)onRecognizedEvent:(IntentRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventListenerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((IntentRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(IntentRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventListenerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((IntentRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(IntentRecognitionCanceledEventArgs *)eventArgs
{
    NSLog(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventListenerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((IntentRecognitionCanceledEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventListener:(IntentRecognitionEventHandlerBlock)eventHandler
{
    [recognizedLock lock];
    [recognizedEventListenerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventListener:(IntentRecognitionEventHandlerBlock)eventHandler
{
    [recognizingLock lock];
    [recognizingEventListenerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventListener:(IntentRecognitionCanceledEventHandlerBlock)eventHandler
{
    [canceledLock lock];
    [canceledEventListenerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

@end
