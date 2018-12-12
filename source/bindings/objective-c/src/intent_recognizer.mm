//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

struct IntentEventHandlerHelper
{
    SPXIntentRecognizer *recognizer;
    IntentRecoSharedPtr recoImpl;
    
    IntentEventHandlerHelper(SPXIntentRecognizer *reco, IntentRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        LogDebug(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
            {
                SPXIntentRecognitionEventArgs *eventArgs = [[SPXIntentRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizedEvent: eventArgs];
            });
    }
    
    void addRecognizingEventHandler()
    {
        LogDebug(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
            {
                SPXIntentRecognitionEventArgs *eventArgs = [[SPXIntentRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizingEvent: eventArgs];
            });
    }
    
    void addCanceledEventHandler()
    {
        LogDebug(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const IntentImpl::IntentRecognitionCanceledEventArgs& e)
            {
                SPXIntentRecognitionCanceledEventArgs *eventArgs = [[SPXIntentRecognitionCanceledEventArgs alloc] init:e];
                [recognizer onCanceledEvent: eventArgs];
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

@implementation SPXIntentRecognizer
{
    IntentRecoSharedPtr intentRecoImpl;
    dispatch_queue_t dispatchQueue;
    
    NSMutableArray *recognizedEventHandlerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventHandlerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventHandlerList;
    NSLock *canceledLock;
    
    struct IntentEventHandlerHelper *eventImpl;
}

- (instancetype)init:(SPXSpeechConfiguration *)speechConfiguration
{
    try {
        auto recoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXIntentRecognizer in core.");
    }
    return nil;
}

- (instancetype)initWithSpeechConfiguration:(SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfiguration getHandle], [audioConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (...) {
        // Todo: better error handling.
        NSLog(@"Exception caught when creating SPXIntentRecognizer in core.");
    }
    return nil;
}

- (instancetype)initWithImpl:(IntentRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->intentRecoImpl = recoHandle;
    if (!self || intentRecoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        
        recognizedEventHandlerList = [NSMutableArray array];
        recognizingEventHandlerList = [NSMutableArray array];
        canceledEventHandlerList = [NSMutableArray array];
        recognizedLock = [[NSLock alloc] init];
        recognizingLock = [[NSLock alloc] init];
        canceledLock = [[NSLock alloc] init];
        
        eventImpl = new IntentEventHandlerHelper(self, intentRecoImpl);
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
    intentRecoImpl->SetAuthorizationToken([token string]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:intentRecoImpl->GetAuthorizationToken()];
}

- (void)addIntentFromPhrase:(NSString *)simplePhrase
{
    intentRecoImpl->AddIntent([simplePhrase string]);
}

- (void)addIntentFromPhrase:(NSString *)simplePhrase mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddIntent([simplePhrase string], [intentId string]);
}

- (void)addIntent: (NSString *)intentName fromModel:(SPXLanguageUnderstandingModel *)model
{
    intentRecoImpl->AddIntent([model getModelHandle], [intentName string]);
}

- (void)addIntent: (NSString *)intentName fromModel:(SPXLanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddIntent([model getModelHandle], [intentName string], [intentId string]);
}

- (void)addAllIntentsFromModel:(nonnull SPXLanguageUnderstandingModel *)model
{
    intentRecoImpl->AddAllIntents([model getModelHandle]);
}

- (void)addAllIntentsFromModel:(SPXLanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddAllIntents([model getModelHandle], [intentId string]);
}

- (SPXIntentRecognitionResult *)recognizeOnce
{
    SPXIntentRecognitionResult *result = nil;
    
    if (intentRecoImpl == nullptr) {
        result = [[SPXIntentRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = intentRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXIntentRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXIntentRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SPXIntentRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(SPXIntentRecognitionResult *))resultReceivedHandler
{
    SPXIntentRecognitionResult *result = nil;
    if (intentRecoImpl == nullptr) {
        result = [[SPXIntentRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedHandler(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = intentRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXIntentRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXIntentRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[SPXIntentRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    dispatch_async(dispatchQueue, ^{
        resultReceivedHandler(result);
    });
}

- (void)startContinuousRecognition
{
    if (intentRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null");
        return;
    }
    
    try {
        intentRecoImpl->StartContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)stopContinuousRecognition
{
    if (intentRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null");
        return;
    }
    
    try {
        intentRecoImpl->StopContinuousRecognitionAsync().get();
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
    }
}

- (void)onRecognizedEvent:(SPXIntentRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventHandlerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXIntentRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(SPXIntentRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventHandlerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXIntentRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(SPXIntentRecognitionCanceledEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventHandlerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXIntentRecognitionCanceledEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventHandler:(SPXIntentRecognitionEventHandler)eventHandler
{
    [recognizedLock lock];
    [recognizedEventHandlerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventHandler:(SPXIntentRecognitionEventHandler)eventHandler
{
    [recognizingLock lock];
    [recognizingEventHandlerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventHandler:(SPXIntentRecognitionCanceledEventHandler)eventHandler
{
    [canceledLock lock];
    [canceledEventHandlerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

@end
