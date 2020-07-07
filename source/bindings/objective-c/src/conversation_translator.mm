//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXConversationTranslator
{
    ConversationTranslatorSharedPtr translatorImpl;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)init
{
    try {
        auto recoImpl = TranscriptionImpl::ConversationTranslator::FromConfig();
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
        NSLog(@"Exception caught when creating SPXConversationTranslator in core.");
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
        auto recoImpl = TranscriptionImpl::ConversationTranslator::FromConfig([audioConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXConversationTranslator in core.");
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

- (instancetype)initWithImpl:(ConversationTranslatorSharedPtr)translatorHandle
{
    self = [super init];
    self->translatorImpl = translatorHandle;
    if (!self || translatorImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (std::shared_ptr<TranscriptionImpl::ConversationTranslator>)getHandle
{
    return translatorImpl;
}

- (void)dealloc
{
    LogDebug(@"Conversation translator object deallocated.");
    if (!self->translatorImpl)
    {
        NSLog(@"translatorImpl is nil in conversation translator destructor");
        return;
    }
    try
    {
        self->translatorImpl->SessionStarted.DisconnectAll();
        self->translatorImpl->SessionStopped.DisconnectAll();
        self->translatorImpl->Transcribing.DisconnectAll();
        self->translatorImpl->Transcribed.DisconnectAll();
        self->translatorImpl->Canceled.DisconnectAll();
        self->translatorImpl->TextMessageReceived.DisconnectAll();
        self->translatorImpl->ConversationExpiration.DisconnectAll();
        self->translatorImpl->ParticipantsChanged.DisconnectAll();
        self->translatorImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in conversation translator destructor");
    }
}

- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation nickname:(nonnull NSString *)nickname
{
    __block NSException * exception = nil;
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->JoinConversationAsync([conversation getHandle], [nickname toSpxString]);
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

- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation nickname:(nonnull NSString *)nickname error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self joinConversationAsync:completedHandler conversation:conversation nickname:nickname];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversationId:(nonnull NSString *)conversationId nickname:(nonnull NSString *)nickname language:(nonnull NSString *)language
{
    __block NSException * exception = nil;
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->JoinConversationAsync([conversationId toSpxString], [nickname toSpxString], [language toSpxString]);
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

- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversationId:(nonnull NSString *)conversationId nickname:(nonnull NSString *)nickname language:(nonnull NSString *)language error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self joinConversationAsync:completedHandler conversationId:conversationId nickname:nickname language:language];
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
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->LeaveConversationAsync();
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

- (void)sendTextMessageAsync:(nonnull void (^)(BOOL sent, NSError * _Nullable))completedHandler message:(nonnull NSString *)message
{
    __block NSException * exception = nil;
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->SendTextMessageAsync([message toSpxString]);
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

- (BOOL)sendTextMessageAsync:(nonnull void (^)(BOOL sent, NSError * _Nullable))completedHandler message:(nonnull NSString *)message error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self sendTextMessageAsync:completedHandler message:message];
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
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->StartTranscribingAsync();
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
    if (translatorImpl == nullptr) {
        NSLog(@"SPXConversationTranslator handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversationTranslator handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = translatorImpl->StopTranscribingAsync();
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

- (void)setAuthorizationToken: (NSString *)token
{
    translatorImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:translatorImpl->GetAuthorizationToken()];
}

- (NSString *)participantId
{
    return [NSString StringWithStdString:translatorImpl->GetParticipantId()];
}

- (void)addTranscribedEventHandler:(SPXConversationTranslationEventHandler)eventHandler
{
    translatorImpl->Transcribed.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranslationEventArgs & evt) {
        SPXConversationTranslationEventArgs *eventArgs = [[SPXConversationTranslationEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addTranscribingEventHandler:(SPXConversationTranslationEventHandler)eventHandler
{
    translatorImpl->Transcribing.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranslationEventArgs & evt) {
        SPXConversationTranslationEventArgs *eventArgs = [[SPXConversationTranslationEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXConversationTranslationCanceledEventHandler)eventHandler
{
    translatorImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranslationCanceledEventArgs & evt) {
        SPXConversationTranslationCanceledEventArgs *eventArgs = [[SPXConversationTranslationCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStartedEventHandler:(SPXConversationSessionEventHandler)eventHandler
{
    translatorImpl->SessionStarted.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStoppedEventHandler:(SPXConversationSessionEventHandler)eventHandler
{
    translatorImpl->SessionStopped.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addTextMessageReceivedEventHandler:(SPXConversationTranslationEventHandler)eventHandler
{
    translatorImpl->TextMessageReceived.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationTranslationEventArgs & evt) {
        SPXConversationTranslationEventArgs *eventArgs = [[SPXConversationTranslationEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addConversationParticipantsChangedEventHandler:(SPXConversationParticipantsChangedEventHandler)eventHandler
{
    translatorImpl->ParticipantsChanged.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationParticipantsChangedEventArgs & evt) {
        SPXConversationParticipantsChangedEventArgs *eventArgs = [[SPXConversationParticipantsChangedEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addConversationExpirationEventHandler:(SPXConversationExpirationEventHandler)eventHandler
{
    translatorImpl->ConversationExpiration.Connect(^(const Microsoft::CognitiveServices::Speech::Transcription::ConversationExpirationEventArgs & evt) {
        SPXConversationExpirationEventArgs *eventArgs = [[SPXConversationExpirationEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

@end
