//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct TranslationEventHandlerHelper
{
    SPXTranslationRecognizer *recognizer;
    TranslationRecoSharedPtr recoImpl;
    
    TranslationEventHandlerHelper(SPXTranslationRecognizer *reco, TranslationRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        LogDebug(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const TranslationImpl::TranslationRecognitionEventArgs& e)
                                     {
                                         SPXTranslationRecognitionEventArgs *eventArgs = [[SPXTranslationRecognitionEventArgs alloc] init: e];
                                         [recognizer onRecognizedEvent: eventArgs];
                                     });
    }
    
    void addRecognizingEventHandler()
    {
        LogDebug(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const TranslationImpl::TranslationRecognitionEventArgs& e)
                                      {
                                          SPXTranslationRecognitionEventArgs *eventArgs = [[SPXTranslationRecognitionEventArgs alloc] init: e];
                                          [recognizer onRecognizingEvent: eventArgs];
                                      });
    }
    
    void addCanceledEventHandler()
    {
        LogDebug(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const TranslationImpl::TranslationRecognitionCanceledEventArgs& e)
                                   {
                                       SPXTranslationRecognitionCanceledEventArgs *eventArgs = [[SPXTranslationRecognitionCanceledEventArgs alloc] init:e];
                                       [recognizer onCanceledEvent: eventArgs];
                                   });
    }
    
    void addSynthesizingEventHandler()
    {
        LogDebug(@"Add SynthesisResultEventHandler");
        recoImpl->Synthesizing.Connect([this] (const TranslationImpl::TranslationSynthesisEventArgs& e)
            {
                SPXTranslationSynthesisEventArgs *eventArgs = [[SPXTranslationSynthesisEventArgs alloc] init: e];
                [recognizer onSynthesizingEvent: eventArgs];
            });
    }

    void addSessionStartedEventHandler()
    {
        LogDebug(@"Add SessionStartedEventHandler");
        recoImpl->SessionStarted.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init:e];
                [recognizer onSessionStartedEvent: eventArgs];
            });
    }
    
    void addSessionStoppedEventHandler()
    {
        LogDebug(@"Add SessionStoppedEventHandler");
        recoImpl->SessionStopped.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init:e];
                [recognizer onSessionStoppedEvent: eventArgs];
            });
    }
    
    void addSpeechStartDetectedEventHandler()
    {
        LogDebug(@"Add SpeechStartDetectedEventHandler");
        recoImpl->SpeechStartDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init:e];
                [recognizer onSpeechStartDetectedEvent: eventArgs];
            });
    }
    
    void addSpeechEndDetectedEventHandler()
    {
        LogDebug(@"Add SpeechStopDetectedEventHandler");
        recoImpl->SpeechEndDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init:e];
                [recognizer onSpeechEndDetectedEvent: eventArgs];
            });
    }
};


@implementation SPXTranslationRecognizer
{
    TranslationRecoSharedPtr translationRecoImpl;
    dispatch_queue_t dispatchQueue;

    NSMutableArray *recognizedEventHandlerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventHandlerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventHandlerList;
    NSLock *canceledLock;
    NSMutableArray *synthesisResultEventHandlerList;
    NSLock *synthesizingLock;
    
    struct TranslationEventHandlerHelper *eventImpl;
}

- (instancetype)init:(SPXSpeechTranslationConfiguration *)translationConfiguration
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translationConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXTranslationRecognizer in core.");
    }
    return nil;
}

- (instancetype)initWithSpeechTranslationConfiguration:(SPXSpeechTranslationConfiguration *)translationConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translationConfiguration getHandle], [audioConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXTranslationRecognizer in core.");
    }
    return nil;
}

- (instancetype)initWithImpl:(TranslationRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->translationRecoImpl = recoHandle;
    if (!self || translationRecoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        
        recognizedEventHandlerList = [NSMutableArray array];
        recognizingEventHandlerList = [NSMutableArray array];
        canceledEventHandlerList = [NSMutableArray array];
        synthesisResultEventHandlerList = [NSMutableArray array];
        recognizedLock = [[NSLock alloc] init];
        recognizingLock = [[NSLock alloc] init];
        canceledLock = [[NSLock alloc] init];
        synthesizingLock = [[NSLock alloc] init];
        
        eventImpl = new TranslationEventHandlerHelper(self, translationRecoImpl);
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
    translationRecoImpl->SetAuthorizationToken([token string]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:translationRecoImpl->GetAuthorizationToken()];
}

- (SPXTranslationRecognitionResult *)recognizeOnce
{
    SPXTranslationRecognitionResult *result = nil;
    
    if (translationRecoImpl == nullptr) {
        result = [[SPXTranslationRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = translationRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXTranslationRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXTranslationRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SPXTranslationRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(SPXTranslationRecognitionResult *))resultReceivedHandler
{
    SPXTranslationRecognitionResult *result = nil;
    if (translationRecoImpl == nullptr) {
        result = [[SPXTranslationRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedHandler(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = translationRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXTranslationRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXTranslationRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SPXTranslationRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    dispatch_async(dispatchQueue, ^{
        resultReceivedHandler(result);
    });
}

- (void)startContinuousRecognition
{
    if (translationRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null");
        return;
    }
    
    try {
        translationRecoImpl->StartContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)stopContinuousRecognition
{
    if (translationRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null");
        return;
    }
    
    try {
        translationRecoImpl->StopContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)onRecognizedEvent:(SPXTranslationRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventHandlerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXTranslationRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(SPXTranslationRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventHandlerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXTranslationRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onSynthesizingEvent:(SPXTranslationSynthesisEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onSynthesisResultEvent");
    NSArray* workCopyOfList;
    [synthesizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:synthesisResultEventHandlerList];
    [synthesizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXTranslationSynthesisEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(SPXTranslationRecognitionCanceledEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventHandlerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXTranslationRecognitionCanceledEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventHandler:(SPXTranslationRecognitionEventHandler)eventHandler
{
    [recognizedLock lock];
    [recognizedEventHandlerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventHandler:(SPXTranslationRecognitionEventHandler)eventHandler
{
    [recognizingLock lock];
    [recognizingEventHandlerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventHandler:(SPXTranslationRecognitionCanceledEventHandler)eventHandler
{
    [canceledLock lock];
    [canceledEventHandlerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

- (void)addSynthesizingEventHandler:(SPXTranslationSynthesisEventHandler)eventHandler
{
    [synthesizingLock lock];
    [synthesisResultEventHandlerList addObject:eventHandler];
    [synthesizingLock unlock];
    return;
}
@end
