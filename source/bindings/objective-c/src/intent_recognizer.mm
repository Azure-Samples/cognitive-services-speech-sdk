//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognizer_private.h"
#import "intent_recognizer_private.h"
#import "intent_recognition_result_private.h"
#import "intent_recognition_event_args_private.h"
#import "session_event_args_private.h"
#import "recognition_event_args_private.h"
#import "recognition_error_event_args_private.h"
#import "common_private.h"

struct IntentEventHandlerHelper
{
    IntentRecognizer *recognizer;
    IntentRecoSharedPtr recoImpl;

    IntentEventHandlerHelper(IntentRecognizer *reco, IntentRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addFinalResultEventHandler()
    {
        NSLog(@"Add FinalResultEventHandler");
        recoImpl->FinalResult.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
            {
                IntentRecognitionEventArgs *eventArgs = [[IntentRecognitionEventArgs alloc] init: e];
                [recognizer onFinalResultEvent: eventArgs];
            });
    }
    
    void addIntermediateResultEventHandler()
    {
        NSLog(@"Add IntermediateResultEventHandler");
        recoImpl->IntermediateResult.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
            {
                IntentRecognitionEventArgs *eventArgs = [[IntentRecognitionEventArgs alloc] init: e];
                [recognizer onIntermediateResultEvent: eventArgs];
            });
    }

    void addErrorEventHandler()
    {
        NSLog(@"Add ErrorEventHandler");
        recoImpl->Canceled.Connect([this] (const IntentImpl::IntentRecognitionEventArgs& e)
            {
                NSString* sessionId = [NSString stringWithString:e.SessionId];
                auto result = e.GetResult();
                NSString* failureReason = [NSString stringWithString:result->ErrorDetails];
                RecognitionStatus status;
                switch (result->Reason)
                {
                case SpeechImpl::Reason::Recognized:
                    status = RecognitionStatus::Recognized;
                    break;
                case SpeechImpl::Reason::IntermediateResult:
                    status = RecognitionStatus::IntermediateResult;
                    break;
                case SpeechImpl::Reason::NoMatch:
                    status = RecognitionStatus::NoMatch;
                    break;
                case SpeechImpl::Reason::InitialSilenceTimeout:
                    status = RecognitionStatus::InitialSilenceTimeout;
                    break;
                case SpeechImpl::Reason::InitialBabbleTimeout:
                    status = RecognitionStatus::InitialBabbleTimeout;
                    break;
                case SpeechImpl::Reason::Canceled:
                    status = RecognitionStatus::Canceled;
                    break;
                default:
                    // Todo error handling.
                    NSLog(@"Unknown recognition status");
                    status = RecognitionStatus::Canceled;
                        failureReason = @"Unexpected status error.";
                    break;
                }
                RecognitionErrorEventArgs *eventArgs = [[RecognitionErrorEventArgs alloc] init:sessionId :status :failureReason];
                [recognizer onErrorEvent: eventArgs];
            });
    }

    void addSessionEventHandler()
    {
        NSLog(@"Add SessionEventHandler");
        recoImpl->SessionStarted.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] init :SessionStartedEvent :e];
                [recognizer onSessionEvent: eventArgs];
            });
        
        recoImpl->SessionStopped.Connect([this] (const SpeechImpl::SessionEventArgs& e)
            {
                SessionEventArgs *eventArgs = [[SessionEventArgs alloc] init :SessionStoppedEvent :e];
                [recognizer onSessionEvent: eventArgs];
            });
    }
    
    void addRecognitionEventHandler()
    {
        NSLog(@"Add RecognitionEventHandler");
        recoImpl->SpeechStartDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] init :SpeechStartDetectedEvent :e];
                [recognizer onRecognitionEvent: eventArgs];
            });
        
        recoImpl->SpeechEndDetected.Connect([this] (const SpeechImpl::RecognitionEventArgs& e)
            {
                RecognitionEventArgs *eventArgs = [[RecognitionEventArgs alloc] init :SpeechEndDetectedEvent : e];
                [recognizer onRecognitionEvent: eventArgs];
            });
    }
};

@implementation IntentRecognizer
{
    IntentRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;
    NSMutableArray *finalResultEventListenerList;
    NSMutableArray *intermediateResultEventListenerList;
    struct IntentEventHandlerHelper *eventImpl;
    NSLock *arrayLock;
}

- (instancetype)init :(IntentRecoSharedPtr)recoHandle
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
        
        eventImpl = new IntentEventHandlerHelper(self, recoImpl);
        [super setDispatchQueue: dispatchQueue];
        eventImpl->addIntermediateResultEventHandler();
        eventImpl->addFinalResultEventHandler();
        eventImpl->addErrorEventHandler();
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

- (void)AddIntentUsingId: (NSString *)intentId ForPhrase: (NSString *)simplePhrase
{
    recoImpl->AddIntent([intentId string], [simplePhrase string]);
}

- (void)AddIntentUsingId: (NSString *)intentId FromModel: (LanguageUnderstandingModel *)model
{
    recoImpl->AddIntent([intentId string], [model getModelHandle]);
}

- (void)AddIntentUsingId: (NSString *)intentId FromModel: (LanguageUnderstandingModel *)model WithIntentName: (NSString *)intentName
{
    recoImpl->AddIntent([intentId string], [model getModelHandle], [intentName string]);
}

- (IntentRecognitionResult *)recognize
{
    IntentRecognitionResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[IntentRecognitionResult alloc] initWithError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
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

- (void)recognizeAsync:(void (^)(IntentRecognitionResult *))resultReceivedBlock
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
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = recoImpl->RecognizeAsync().get();
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

- (void)close
{
    if (recoImpl != nullptr) {
        recoImpl.reset();
    }
}

- (void)onFinalResultEvent:(IntentRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onFinalResultEvent");
    NSArray* workCopyOfList;
    [arrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:finalResultEventListenerList];
    [arrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((IntentRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onIntermediateResultEvent:(IntentRecognitionEventArgs *)eventArgs
{
    NSLog(@"OBJC: onIntermediateResultEvent");
    NSArray* workCopyOfList;
    [arrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:intermediateResultEventListenerList];
    [arrayLock unlock];
    for (id handle in intermediateResultEventListenerList) {
        dispatch_async(dispatchQueue, ^{
            ((IntentRecognitionEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addFinalResultEventListener:(IntentRecognitionEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    [finalResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}

- (void)addIntermediateResultEventListener:(IntentRecognitionEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    [intermediateResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}

@end
