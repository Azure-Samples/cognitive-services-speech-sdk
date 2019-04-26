//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

struct SpeechEventHandlerHelper
{
    SPXSpeechRecognizer *recognizer;
    SpeechRecoSharedPtr recoImpl;

    SpeechEventHandlerHelper(SPXSpeechRecognizer *reco, SpeechRecoSharedPtr recoImpl)
    {
        recognizer = reco;
        this->recoImpl = recoImpl;
    }
    
    void addRecognizedEventHandler()
    {
        LogDebug(@"Add RecognizedEventHandler");
        recoImpl->Recognized.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SPXSpeechRecognitionEventArgs *eventArgs = [[SPXSpeechRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizedEvent: eventArgs];
            });
    }
    
    void addRecognizingEventHandler()
    {
        LogDebug(@"Add RecognizingEventHandler");
        recoImpl->Recognizing.Connect([this] (const SpeechImpl::SpeechRecognitionEventArgs& e)
            {
                SPXSpeechRecognitionEventArgs *eventArgs = [[SPXSpeechRecognitionEventArgs alloc] init: e];
                [recognizer onRecognizingEvent: eventArgs];
            });
    }

    void addCanceledEventHandler()
    {
        LogDebug(@"Add CanceledEventHandler");
        recoImpl->Canceled.Connect([this] (const SpeechImpl::SpeechRecognitionCanceledEventArgs& e)
            {
                SPXSpeechRecognitionCanceledEventArgs *eventArgs = [[SPXSpeechRecognitionCanceledEventArgs alloc] init:e];
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

@implementation SPXSpeechRecognizer
{
    SpeechRecoSharedPtr speechRecoImpl;
    dispatch_queue_t dispatchQueue;
    
    NSMutableArray *recognizedEventHandlerList;
    NSLock *recognizedLock;
    NSMutableArray *recognizingEventHandlerList;
    NSLock *recognizingLock;
    NSMutableArray *canceledEventHandlerList;
    NSLock *canceledLock;
    struct SpeechEventHandlerHelper *eventImpl;
    
    RecognizerPropertyCollection *propertyCollection;
}

- (instancetype)init:(SPXSpeechConfiguration *)speechConfiguration {
     try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXSpeechRecognizer in core.\nNOTE: This will raise an exception in the future!");
    }
    return nil;
}


- (instancetype)initWithSpeechConfiguration:(SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration {
    try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfiguration getHandle], [audioConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXSpeechRecognizer in core.\nNOTE: This will raise an exception in the future!");
    }
    return nil;
}

- (instancetype)initWithImpl:(SpeechRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->speechRecoImpl = recoHandle;
    if (!self || speechRecoImpl == nullptr) {
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
        
        eventImpl = new SpeechEventHandlerHelper(self, speechRecoImpl);
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

- (void)dealloc {
    NSLog(@"Speech recognizer object deallocated.");
    if (!self->speechRecoImpl)
    {
        NSLog(@"speechRecoImpl is nil in speech recognizer destructor");
        return;
    }
    
    try 
    {
        self->speechRecoImpl->SessionStarted.DisconnectAll();
        self->speechRecoImpl->SessionStopped.DisconnectAll();
        self->speechRecoImpl->SpeechStartDetected.DisconnectAll();
        self->speechRecoImpl->SpeechEndDetected.DisconnectAll();
        self->speechRecoImpl->Recognizing.DisconnectAll();
        self->speechRecoImpl->Recognized.DisconnectAll();
        self->speechRecoImpl->Canceled.DisconnectAll();
        self->speechRecoImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in speech recognizer destructor");
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    speechRecoImpl->SetAuthorizationToken([token string]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:speechRecoImpl->GetAuthorizationToken()];
}

- (NSString *)endpointId
{
    return [NSString StringWithStdString:speechRecoImpl->GetEndpointId()];
}

- (SPXSpeechRecognitionResult *)recognizeOnce
{
    SPXSpeechRecognitionResult *result = nil;
    
    if (speechRecoImpl == nullptr) {
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        return result;
    }
    
    try {
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = speechRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXSpeechRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXSpeechRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
        // [exception raise];
    }
    catch (...) {
        NSLog(@"%@: Exception caught\nNOTE: This will raise an exception in the future!", NSStringFromSelector(_cmd));
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    return result;
}

- (void)recognizeOnceAsync:(void (^)(SPXSpeechRecognitionResult *))resultReceivedHandler
{
    SPXSpeechRecognitionResult *result = nil;
    if (speechRecoImpl == nullptr) {
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedHandler(result);
        });
        return;
    }
    
    try {
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = speechRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            result = [[SPXSpeechRecognitionResult alloc] initWithError: @"No result available."];
        }
        else
        {
            result = [[SPXSpeechRecognitionResult alloc] init: resultImpl];
        }
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
        // [exception raise];
    }
    catch (...) {
        NSLog(@"%@: Exception caught\nNOTE: This will raise an exception in the future!", NSStringFromSelector(_cmd));
        result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
    }
    
    dispatch_async(dispatchQueue, ^{
        resultReceivedHandler(result);
    });
}

- (void)startContinuousRecognition
{
    if (speechRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null\nNOTE: This will raise an exception in the future!");
        return;
    }
    
    try {
        speechRecoImpl->StartContinuousRecognitionAsync().get();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught in startContinuousRecognition\nNOTE: This will raise an exception in the future!");
    }
}

- (void)stopContinuousRecognition
{
    if (speechRecoImpl == nullptr) {
        // Todo: return error?
        NSLog(@"SPXRecognizer handle is null\nNOTE: This will raise an exception in the future!");
        return;
    }
    
    try {
        speechRecoImpl->StopContinuousRecognitionAsync().get();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        UNUSED(exception);
        // [exception raise];
    }
    catch (...) {
        NSLog(@"%@: Exception caught\nNOTE: This will raise an exception in the future!", NSStringFromSelector(_cmd));
    }
}

- (void)onRecognizedEvent:(SPXSpeechRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizedEvent");
    NSArray* workCopyOfList;
    [recognizedLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizedEventHandlerList];
    [recognizedLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXSpeechRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onRecognizingEvent:(SPXSpeechRecognitionEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onRecognizingEvent");
    NSArray* workCopyOfList;
    [recognizingLock lock];
    workCopyOfList = [NSArray arrayWithArray:recognizingEventHandlerList];
    [recognizingLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXSpeechRecognitionEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)onCanceledEvent:(SPXSpeechRecognitionCanceledEventArgs *)eventArgs
{
    LogDebug(@"OBJC: onCanceledEvent");
    NSArray* workCopyOfList;
    [canceledLock lock];
    workCopyOfList = [NSArray arrayWithArray:canceledEventHandlerList];
    [canceledLock unlock];
    for (id handle in workCopyOfList) {
        dispatch_async(dispatchQueue, ^{
            ((SPXSpeechRecognitionCanceledEventHandler)handle)(self, eventArgs);
        });
    }
}

- (void)addRecognizedEventHandler:(SPXSpeechRecognitionEventHandler)eventHandler
{
    [recognizedLock lock];
    [recognizedEventHandlerList addObject:eventHandler];
    [recognizedLock unlock];
    return;
}

- (void)addRecognizingEventHandler:(SPXSpeechRecognitionEventHandler)eventHandler
{
    [recognizingLock lock];
    [recognizingEventHandlerList addObject:eventHandler];
    [recognizingLock unlock];
    return;
}

- (void)addCanceledEventHandler:(SPXSpeechRecognitionCanceledEventHandler)eventHandler
{
    [canceledLock lock];
    [canceledEventHandlerList addObject:eventHandler];
    [canceledLock unlock];
    return;
}

@end
