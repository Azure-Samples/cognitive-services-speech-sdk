//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct TranslationEventHandlerHelper
{
    TranslationRecognizer *recognizer;
    TranslationRecoSharedPtr recoImpl;
    
    TranslationEventHandlerHelper(TranslationRecognizer *reco, TranslationRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        NSLog(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const TranslationImpl::TranslationRecognitionEventArgs& e)
                                     {
                                         TranslationRecognitionEventArgs *eventArgs = [[TranslationRecognitionEventArgs alloc] init: e];
                                         [recognizer onRecognizedEvent: eventArgs];
                                     });
    }
    
    void addRecognizingEventHandler()
    {
        NSLog(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const TranslationImpl::TranslationRecognitionEventArgs& e)
                                      {
                                          TranslationRecognitionEventArgs *eventArgs = [[TranslationRecognitionEventArgs alloc] init: e];
                                          [recognizer onRecognizingEvent: eventArgs];
                                      });
    }
    
    void addCanceledEventHandler()
    {
        NSLog(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const TranslationImpl::TranslationRecognitionCanceledEventArgs& e)
                                   {
                                       TranslationRecognitionCanceledEventArgs *eventArgs = [[TranslationRecognitionCanceledEventArgs alloc] init:e];
                                       [recognizer onCanceledEvent: eventArgs];
                                   });
    }
    
    void addSynthesizingEventHandler()
    {
        NSLog(@"Add SynthesisResultEventHandler");
        recoImpl->Synthesizing.Connect([this] (const TranslationImpl::TranslationSynthesisEventArgs& e)
            {
                TranslationSynthesisEventArgs *eventArgs = [[TranslationSynthesisEventArgs alloc] init: e];
                [recognizer onSynthesizingEvent: eventArgs];
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


@implementation TranslationRecognizer
{
    TranslationRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;

    NSMutableArray *recognizedEventListenerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventListenerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventListenerList;
    NSLock *canceledLock;
    NSMutableArray *synthesisResultEventListenerList;
    NSLock *synthesizingLock;
    
    struct TranslationEventHandlerHelper *eventImpl;
}

+ (TranslationRecognizer *)fromConfig: (SpeechTranslationConfig *)translatorConfig
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translatorConfig getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [[TranslationRecognizer alloc] init :recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating TranslationRecognizer in core.");
    }
    return nil;
}

+ (TranslationRecognizer *)fromConfig: (SpeechTranslationConfig *)translatorConfig usingAudio: (AudioConfig *)audioConfig
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translatorConfig getHandle], [audioConfig getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [[TranslationRecognizer alloc] init :recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating TranslationRecognizer in core.");
    }
    return nil;
}

- (instancetype)init :(TranslationRecoSharedPtr)recoHandle
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
        synthesisResultEventListenerList = [NSMutableArray array];
        recognizedLock = [[NSLock alloc] init];
        recognizingLock = [[NSLock alloc] init];
        canceledLock = [[NSLock alloc] init];
        synthesizingLock = [[NSLock alloc] init];
        
        eventImpl = new TranslationEventHandlerHelper(self, recoImpl);
        [super setDispatchQueue: dispatchQueue];
        eventImpl->addRecognizingEventHandler();
        eventImpl->addRecognizedEventHandler();
        eventImpl->addSynthesizingEventHandler();
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

- (TranslationRecognitionResult *)recognizeOnce
{
    TranslationRecognitionResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[TranslationRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[TranslationRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[TranslationRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[TranslationRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(TranslationRecognitionResult *))resultReceivedBlock
{
    TranslationRecognitionResult *result = nil;
    if (recoImpl == nullptr) {
        result = [[TranslationRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedBlock(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = recoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[TranslationRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[TranslationRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[TranslationRecognitionResult alloc] initWithError: @"Runtime Exception"];
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

- (void)onRecognizedEvent:(TranslationRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventListenerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationRecognitionResultEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(TranslationRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventListenerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationRecognitionResultEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onSynthesizingEvent:(TranslationSynthesisEventArgs *)eventArgs
{
    NSLog(@"OBJC: onSynthesisResultEvent");
    NSArray* workCopyOfList;
    [synthesizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:synthesisResultEventListenerList];
    [synthesizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationSynthesisResultEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(TranslationRecognitionCanceledEventArgs *)eventArgs
{
    NSLog(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventListenerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationRecognitionResultCanceledEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventListener:(TranslationRecognitionResultEventHandlerBlock)eventHandler
{
    [recognizedLock lock];
    [recognizedEventListenerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventListener:(TranslationRecognitionResultEventHandlerBlock)eventHandler
{
    [recognizingLock lock];
    [recognizingEventListenerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventListener:(TranslationRecognitionResultCanceledEventHandlerBlock)eventHandler
{
    [canceledLock lock];
    [canceledEventListenerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

- (void)addSynthesizingEventListener:(TranslationSynthesisResultEventHandlerBlock)eventHandler
{
    [synthesizingLock lock];
    [synthesisResultEventListenerList addObject:eventHandler];
    [synthesizingLock unlock];
    return;
}
@end
