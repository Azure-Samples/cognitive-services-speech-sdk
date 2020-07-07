//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXConversation
{
    ConversationSharedPtr conversationHandle;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)initWithImpl:(ConversationSharedPtr)conversationImpl
{
    self = [super init];
    self->conversationHandle = conversationImpl;
    if (!self || self->conversationHandle == nullptr) {
        NSLog(@"conversationHandle is nil in SPXConversation constructor");
        return nil;
    }
    else
    {
        NSLog(@"conversationHandle is not nil in SPXConversation constructor");
        _properties = [[ConversationPropertyCollection alloc] initWithPropertyCollection:&conversationHandle->Properties from:conversationHandle];
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (instancetype)initWithError:(NSString *)message
{
    conversationHandle = nullptr;
    return self;
}

- (std::shared_ptr<TranscriptionImpl::Conversation>)getHandle
{
    return conversationHandle;
}

- (void)dealloc
{
    LogDebug(@"conversation object deallocated.");
    if (!self->conversationHandle)
    {
        NSLog(@"conversationHandle is nil in SPXConversation destructor");
        return;
    }
    
    try
    {
        conversationHandle.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...)
    {
        NSLog(@"Exception caught in SPXConversation destructor");
    }
}

- (instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration
{
    try {
        std::shared_ptr<TranscriptionImpl::Conversation> conversationImpl = conversationHandle->CreateConversationAsync([speechConfiguration getHandle], [@"" toSpxString]).get();
        if (conversationImpl == nullptr) {
            NSLog(@"SPXConversation object is nil after creation");
            return nil;
        }
        else {
            auto conversation = [[SPXConversation alloc] initWithImpl:conversationImpl];
            return conversation;
        }
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
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self init:speechConfiguration];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration conversationId:(nonnull NSString *)conversationId
{
    try {
        std::shared_ptr<TranscriptionImpl::Conversation> conversationImpl = conversationHandle->CreateConversationAsync([speechConfiguration getHandle], [conversationId toSpxString]).get();
        if (conversationImpl == nullptr) {
            NSLog(@"SPXConversation object is nil after creation");
            return nil;
        }
        else {
            auto conversation = [[SPXConversation alloc] initWithImpl:conversationImpl];
            return conversation;
        }
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
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration conversationId:(nonnull NSString *)conversationId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self init:speechConfiguration conversationId:conversationId];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (void)startConversationAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->StartConversationAsync();
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

- (BOOL)startConversationAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self startConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)endConversationAsync:(nonnull void (^)(BOOL ended, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->EndConversationAsync();
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

- (BOOL)endConversationAsync:(nonnull void (^)(BOOL ended, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self endConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)deleteConversationAsync:(nonnull void (^)(BOOL deleted, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->DeleteConversationAsync();
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

- (BOOL)deleteConversationAsync:(nonnull void (^)(BOOL deleted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self deleteConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)lockConversationAsync:(nonnull void (^)(BOOL locked, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->LockConversationAsync();
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

- (BOOL)lockConversationAsync:(nonnull void (^)(BOOL locked, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self lockConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)unlockConversationAsync:(nonnull void (^)(BOOL unlocked, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->UnlockConversationAsync();
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

- (BOOL)unlockConversationAsync:(nonnull void (^)(BOOL unlocked, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self unlockConversationAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)muteAllParticipantsAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->MuteAllParticipantsAsync();
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

- (BOOL)muteAllParticipantsAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self muteAllParticipantsAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)unmuteAllParticipantsAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->UnmuteAllParticipantsAsync();
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

- (BOOL)unmuteAllParticipantsAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self unmuteAllParticipantsAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)muteParticipantAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->MuteParticipantAsync([userId toSpxString]);
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

- (BOOL)muteParticipantAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self muteParticipantAsync:completedHandler userId:userId];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)unmuteParticipantAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try{
        futureObj = conversationHandle->UnmuteParticipantAsync([userId toSpxString]);
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

- (BOOL)unmuteParticipantAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self unmuteParticipantAsync:completedHandler userId:userId];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }

    __block std::future<std::shared_ptr<TranscriptionImpl::Participant>> futureObj;
    try{
        futureObj = conversationHandle->AddParticipantAsync([userId toSpxString]);
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
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    dispatch_async(dispatchQueue, ^{
        try {
            std::shared_ptr<TranscriptionImpl::Participant> participantImpl = futureObj.get();
            if (participantImpl == nullptr) {
                NSLog(@"Participant is null");
                exception = [NSException exceptionWithName:@"SPXException"
                                                    reason:@"Participant is null"
                                                  userInfo:nil];
            }
            else
            {
                auto participant = [[SPXParticipant alloc] initWithImpl: participantImpl];
                completedHandler(participant, nil);
                return;
            }
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s", e.what());
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
        completedHandler(nil, [Util getErrorFromException:exception]);
    });
}

- (BOOL)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self addParticipantAsync:completedHandler userId:userId];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXConversation handle is null"
                                                       userInfo:nil];
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }

    __block std::future<std::shared_ptr<TranscriptionImpl::Participant>> futureObj;
    try{
        futureObj = conversationHandle->AddParticipantAsync([participant getHandle]);
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
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    dispatch_async(dispatchQueue, ^{
        try {
            std::shared_ptr<TranscriptionImpl::Participant> participantImpl = futureObj.get();
            if (participantImpl == nullptr) {
                NSLog(@"Participant is null");
                exception = [NSException exceptionWithName:@"SPXException"
                                                    reason:@"Participant is null"
                                                  userInfo:nil];
            }
            else
            {
                auto participant = [[SPXParticipant alloc] initWithImpl: participantImpl];
                completedHandler(participant, nil);
                return;
            }
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s", e.what());
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
        completedHandler(nil, [Util getErrorFromException:exception]);
    });
}

- (BOOL)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self addParticipantAsync:completedHandler participant:participant];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)addParticipantAsync:(nonnull void (^)(SPXUser * _Nullable, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXUser handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXUser handle is null"
                                                       userInfo:nil];
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<std::shared_ptr<TranscriptionImpl::User>> futureObj;
    try{
        futureObj = conversationHandle->AddParticipantAsync([user getHandle]);
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
        completedHandler(nil, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    dispatch_async(dispatchQueue, ^{
        try {
            std::shared_ptr<TranscriptionImpl::User> userImpl = futureObj.get();
            if (userImpl == nullptr) {
                NSLog(@"User is null");
                exception = [NSException exceptionWithName:@"SPXException"
                                                    reason:@"User is null"
                                                  userInfo:nil];
            }
            else
            {
                auto user = [[SPXUser alloc] initWithImpl: userImpl];
                completedHandler(user, nil);
                return;
            }
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s", e.what());
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
        completedHandler(nil, [Util getErrorFromException:exception]);
    });
}

- (BOOL)addParticipantAsync:(nonnull void (^)(SPXUser * _Nullable, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self addParticipantAsync:completedHandler user:user];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXConversation handle is null"
                                          userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = conversationHandle->RemoveParticipantAsync([userId toSpxString]);
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

- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self removeParticipantAsync:completedHandler userId:userId];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXConversation handle is null"
                                          userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = conversationHandle->RemoveParticipantAsync([participant getHandle]);
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

- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self removeParticipantAsync:completedHandler participant:participant];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user
{
    __block NSException * exception = nil;
    if (conversationHandle == nullptr) {
        NSLog(@"SPXConversation handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXConversation handle is null"
                                          userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = conversationHandle->RemoveParticipantAsync([user getHandle]);
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

- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self removeParticipantAsync:completedHandler user:user];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (NSString *)conversationId
{
    return [NSString StringWithStdString:conversationHandle->GetConversationId()];
}

- (void)setAuthorizationToken: (NSString *)token
{
    conversationHandle->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:conversationHandle->GetAuthorizationToken()];
}

@end
