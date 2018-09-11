//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognizer_private.h"
#import "translation_recognizer_private.h"
#import "translation_text_result_private.h"
#import "translation_text_event_args_private.h"
#import "translation_synthesis_result_private.h"
#import "translation_synthesis_event_args_private.h"
#import "session_event_args_private.h"
#import "recognition_event_args_private.h"
#import "recognition_error_event_args_private.h"
#import "common_private.h"

struct TranslationEventHandlerHelper
{
    TranslationRecognizer *recognizer;
    TranslationRecoSharedPtr recoImpl;

    TranslationEventHandlerHelper(TranslationRecognizer *reco, TranslationRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addFinalResultEventHandler()
    {
        NSLog(@"Add FinalResultEventHandler");
        recoImpl->FinalResult.Connect([this] (const TranslationImpl::TranslationTextResultEventArgs& e)
            {
                TranslationTextEventArgs *eventArgs = [[TranslationTextEventArgs alloc] init: e];
                [recognizer onFinalResultEvent: eventArgs];
            });
    }

    void addIntermediateResultEventHandler()
    {
        NSLog(@"Add IntermediateResultEventHandler");
        recoImpl->IntermediateResult.Connect([this] (const TranslationImpl::TranslationTextResultEventArgs& e)
            {
                TranslationTextEventArgs *eventArgs = [[TranslationTextEventArgs alloc] init: e];
                [recognizer onIntermediateResultEvent: eventArgs];
            });
    }

    void addSynthesisResultEventHandler()
    {
        NSLog(@"Add SynthesisResultEventHandler");
        recoImpl->TranslationSynthesisResultEvent.Connect([this] (const TranslationImpl::TranslationSynthesisResultEventArgs& e)
            {
                TranslationSynthesisEventArgs *eventArgs = [[TranslationSynthesisEventArgs alloc] init: e];
                [recognizer onSynthesisResultEvent: eventArgs];
            });
    }

    void addErrorEventHandler()
    {
        NSLog(@"Add ErrorEventHandler");
        recoImpl->Canceled.Connect([this] (const TranslationImpl::TranslationTextResultEventArgs& e)
            {
                NSString *sessionId = [NSString stringWithString:e.SessionId];
                auto result = e.GetResult();
                NSString *failureReason = [NSString stringWithString:result->ErrorDetails];
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
                    status = RecognitionStatus::Canceled;
                    NSLog(@"Unknown recognition status");
                    break;
                }
                RecognitionErrorEventArgs *eventArgs = [[RecognitionErrorEventArgs alloc] init :sessionId :status :failureReason];
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

@implementation TranslationRecognizer
{
    TranslationRecoSharedPtr recoImpl;
    dispatch_queue_t dispatchQueue;
    NSMutableArray *finalResultEventListenerList;
    NSMutableArray *intermediateResultEventListenerList;
    NSMutableArray *synthesisResultEventListenerList;
    NSMutableArray *errorEventListenerList;
    struct TranslationEventHandlerHelper *eventImpl;
    NSLock *arrayLock;
}

- (instancetype)init :(TranslationRecoSharedPtr)recoHandle
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
        synthesisResultEventListenerList = [NSMutableArray array];
        errorEventListenerList = [NSMutableArray array];
        arrayLock = [[NSLock alloc] init];
        
        eventImpl = new TranslationEventHandlerHelper(self, recoImpl);
        [super setDispatchQueue: dispatchQueue];
        eventImpl->addIntermediateResultEventHandler();
        eventImpl->addFinalResultEventHandler();
        eventImpl->addSynthesisResultEventHandler();
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

- (TranslationTextResult *)translate
{
    TranslationTextResult *result = nil;
    
    if (recoImpl == nullptr) {
        result = [[TranslationTextResult alloc] initWithError: @"Recognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationTextResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[TranslationTextResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[TranslationTextResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[TranslationTextResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)translateAsync:(void (^)(TranslationTextResult *))resultReceivedBlock
{
    TranslationTextResult *result = nil;
    if (recoImpl == nullptr) {
        result = [[TranslationTextResult alloc] initWithError: @"Recognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedBlock(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<TranslationImpl::TranslationTextResult> resultImpl = recoImpl->RecognizeAsync().get();
        if (resultImpl == nullptr) {
            result = [[TranslationTextResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[TranslationTextResult alloc] init: resultImpl];
        }
    }
    catch (...) {
        // Todo: better error handling
        NSLog(@"exception caught");
        result = [[TranslationTextResult alloc] initWithError: @"Runtime Exception"];
    }
    
    dispatch_async(dispatchQueue, ^{
        resultReceivedBlock(result);
    });
}

- (void)startContinuousTranslation
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

- (void)stopContinuousTranslation
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

- (void)onFinalResultEvent:(TranslationTextEventArgs *)eventArgs
{
    NSLog(@"OBJC: onFinalResultEvent");
    NSArray* workCopyOfList;
    [arrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:finalResultEventListenerList];
    [arrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationTextEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onIntermediateResultEvent:(TranslationTextEventArgs *)eventArgs
{
    NSLog(@"OBJC: onIntermediateResultEvent");
    NSArray* workCopyOfList;
    [arrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:intermediateResultEventListenerList];
    [arrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationTextEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)onSynthesisResultEvent:(TranslationSynthesisEventArgs *)eventArgs
{
    NSLog(@"OBJC: onSynthesisResultEvent");
    NSArray* workCopyOfList;
    [arrayLock lock];
    workCopyOfList = [NSArray arrayWithArray:synthesisResultEventListenerList];
    [arrayLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((TranslationSynthesisEventHandlerBlock)handle)(self, eventArgs);
        });
    }
}

- (void)addFinalResultEventListener:(TranslationTextEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    [finalResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}

- (void)addIntermediateResultEventListener:(TranslationTextEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    [intermediateResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}


- (void)addSynthesisResultEventListener:(TranslationSynthesisEventHandlerBlock)eventHandler
{
    [arrayLock lock];
    [synthesisResultEventListenerList addObject:eventHandler];
    [arrayLock unlock];
    return;
}
@end
