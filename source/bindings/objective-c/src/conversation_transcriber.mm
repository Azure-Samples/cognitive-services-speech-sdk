//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXConversationTranscriber
{
    ConversationTranscriberSharedPtr transcriberImpl;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)init
{
    try {
        auto recoImpl = TranscriptionImpl::ConversationTranscriber::FromConfig();
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXConversationTranscriber in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self init];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initWithAudioConfiguration:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = TranscriptionImpl::ConversationTranscriber::FromConfig([audioConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXConversationTranscriber in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithAudioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithAudioConfiguration:audioConfiguration];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initWithImpl:(ConversationTranscriberSharedPtr)transcriberHandle
{
    self = [super initWith:transcriberHandle withParameters:&transcriberHandle->Properties];
    self->transcriberImpl = transcriberHandle;
    if (!self || transcriberImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (void)dealloc
{
    LogDebug(@"Conversation transcriber object deallocated.");
    if (!self->transcriberImpl)
    {
        NSLog(@"transcriberImpl is nil in conversation transcriber destructor");
        return;
    }
    try
    {
        self->transcriberImpl->SessionStarted.DisconnectAll();
        self->transcriberImpl->SessionStopped.DisconnectAll();
        self->transcriberImpl->SpeechStartDetected.DisconnectAll();
        self->transcriberImpl->SpeechEndDetected.DisconnectAll();
        self->transcriberImpl->Transcribing.DisconnectAll();
        self->transcriberImpl->Transcribed.DisconnectAll();
        self->transcriberImpl->Canceled.DisconnectAll();
        self->transcriberImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in conversation transcriber destructor");
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    transcriberImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:transcriberImpl->GetAuthorizationToken()];
}

- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation
{
    __block NSException * exception = nil;
    if (transcriberImpl == nullptr) {
        NSLog(@"SPXConversationTranscriber handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranscriber handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = transcriberImpl->JoinConversationAsync([conversation getHandle]);
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    dispatch_async(dispatchQueue, ^{
        try {
            futureObj.get();
            completedHandler(YES, nil);
            return;
        }
        catch (const std::exception &e) {
            NSLog(@"dispatch_async: Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"dispatch_async: Exception with error code in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (...) {
            NSLog(@"%@: dispatch_async: Exception caught", NSStringFromSelector(_cmd));
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:@"Runtime exception"
                                              userInfo:nil];
        }
        NSError *error = [Util getErrorFromException:exception];
        completedHandler(NO, error);
    });
}

- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self joinConversationAsync:completedHandler conversation:conversation];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (transcriberImpl == nullptr) {
        NSLog(@"SPXConversationTranscriber handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranscriber handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = transcriberImpl->LeaveConversationAsync();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }

    dispatch_async(dispatchQueue, ^{
        try {
            futureObj.get();
            completedHandler(YES, nil);
            return;
        }
        catch (const std::exception &e) {
            NSLog(@"dispatch_async: Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"dispatch_async: Exception with error code in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (...) {
            NSLog(@"%@: dispatch_async: Exception caught", NSStringFromSelector(_cmd));
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:@"Runtime exception"
                                              userInfo:nil];
        }
        NSError *error = [Util getErrorFromException:exception];
        completedHandler(NO, error);
    });
}

- (BOOL)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self leaveConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (transcriberImpl == nullptr) {
        NSLog(@"SPXConversationTranscriber handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranscriber handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = transcriberImpl->StartTranscribingAsync();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    dispatch_async(dispatchQueue, ^{
        try {
            futureObj.get();
            completedHandler(YES, nil);
            return;
        }
        catch (const std::exception &e) {
            NSLog(@"dispatch_async: Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"dispatch_async: Exception with error code in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (...) {
            NSLog(@"%@: dispatch_async: Exception caught", NSStringFromSelector(_cmd));
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:@"Runtime exception"
                                              userInfo:nil];
        }
        NSError *error = [Util getErrorFromException:exception];
        completedHandler(NO, error);
    });
}

- (BOOL)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self startTranscribingAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (transcriberImpl == nullptr) {
        NSLog(@"SPXConversationTranscriber handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranscriber handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = transcriberImpl->StopTranscribingAsync();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    dispatch_async(dispatchQueue, ^{
        try {
            futureObj.get();
            completedHandler(YES, nil);
            return;
        }
        catch (const std::exception &e) {
            NSLog(@"dispatch_async: Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"dispatch_async: Exception with error code in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (...) {
            NSLog(@"%@: dispatch_async: Exception caught", NSStringFromSelector(_cmd));
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:@"Runtime exception"
                                              userInfo:nil];
        }
        NSError *error = [Util getErrorFromException:exception];
        completedHandler(NO, error);
    });
}

- (BOOL)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self stopTranscribingAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)addTranscribedEventHandler:(SPXConversationTranscriptionEventHandler)eventHandler
{
    transcriberImpl->Transcribed.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranscriptionEventArgs & evt) {
        SPXConversationTranscriptionEventArgs *eventArgs = [[SPXConversationTranscriptionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addTranscribingEventHandler:(SPXConversationTranscriptionEventHandler)eventHandler
{
    transcriberImpl->Transcribing.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranscriptionEventArgs & evt) {
        SPXConversationTranscriptionEventArgs *eventArgs = [[SPXConversationTranscriptionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXConversationTranscriptionCanceledEventHandler)eventHandler
{
    transcriberImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranscriptionCanceledEventArgs & evt) {
        SPXConversationTranscriptionCanceledEventArgs *eventArgs = [[SPXConversationTranscriptionCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
    transcriberImpl->SessionStarted.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
    transcriberImpl->SessionStopped.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    transcriberImpl->SpeechStartDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    transcriberImpl->SpeechEndDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

@end
