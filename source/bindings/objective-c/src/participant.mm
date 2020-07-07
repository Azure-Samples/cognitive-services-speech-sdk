//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXParticipant
{
    ParticipantSharedPtr participantHandle;
}

- (instancetype)initFrom:(NSString *)userId preferredLanguage:(NSString *)preferredLanguage voiceSignature:(NSString *)voiceSignature
{
     try {
        auto participantImpl = TranscriptionImpl::Participant::From([userId toSpxString], [preferredLanguage toSpxString], [voiceSignature toSpxString]);
        if (participantImpl == nullptr)
            return nil;
        return [self initWithImpl:participantImpl];
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
        NSLog(@"Exception caught when creating SPXParticipant in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFrom:(NSString *)userId preferredLanguage:(NSString *)preferredLanguage voiceSignature:(NSString *)voiceSignature error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFrom:userId preferredLanguage:preferredLanguage voiceSignature:voiceSignature];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initFrom:(NSString *)userId preferredLanguage:(NSString *)preferredLanguage
{
     try {
        auto participantImpl = TranscriptionImpl::Participant::From([userId toSpxString], [preferredLanguage toSpxString], [@"" toSpxString]);
        if (participantImpl == nullptr)
            return nil;
        return [self initWithImpl:participantImpl];
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
        NSLog(@"Exception caught when creating SPXParticipant in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFrom:(NSString *)userId preferredLanguage:(NSString *)preferredLanguage error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFrom:userId preferredLanguage:preferredLanguage];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initFrom:(NSString *)userId
{
     try {
        auto participantImpl = TranscriptionImpl::Participant::From([userId toSpxString], [@"" toSpxString], [@"" toSpxString]);
        if (participantImpl == nullptr)
            return nil;
        return [self initWithImpl:participantImpl];
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
        NSLog(@"Exception caught when creating SPXParticipant in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFrom:(NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFrom:userId];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initWithImpl:(ParticipantSharedPtr)participantImpl
{
    self = [super init];
    self->participantHandle = participantImpl;
    if (!self || self->participantHandle == nullptr) {
        NSLog(@"participantHandle is nil in SPXParticipant constructor");
        return nil;
    }
    else {
        _properties = [[ParticipantPropertyCollection alloc] initWithPropertyCollection:&participantHandle->Properties from:participantHandle];
        return self;
    }
}

- (ParticipantSharedPtr)getHandle
{
    return participantHandle;
}

- (instancetype)initWithError: (NSString *)message
{
    self->participantHandle = nullptr;
    return self;
}

- (void)dealloc
{
    LogDebug(@"participant object deallocated.");
    if (!self->participantHandle) {
        NSLog(@"participantHandle is nil in SPXParticipant destructor");
        return;
    }

    try {
        participantHandle.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in SPXParticipant destructor");
    }
}

- (NSString *)id
{
    return [NSString StringWithStdString:participantHandle->Id];
}

- (NSString *)avatar
{
    return [NSString StringWithStdString:participantHandle->Avatar];
}

- (NSString *)displayName
{
    return [NSString StringWithStdString:participantHandle->DisplayName];
}

- (void)setPreferredLanguage:(NSString *)preferredLanguage
{
    try {
        participantHandle->SetPreferredLanguage([preferredLanguage toSpxString]);
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
        NSLog(@"Exception caught when creating SPXParticipant in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
}

- (void)setVoiceSignature:(NSString *)voiceSignature
{
    try {
        participantHandle->SetVoiceSignature([voiceSignature toSpxString]);
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
        NSLog(@"Exception caught when creating SPXParticipant in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
}

- (BOOL)isMuted
{
    BOOL result = NO;
    if (participantHandle->IsMuted) {
        result = YES;
    }
    return result;
}

- (BOOL)isUsingTts
{
    BOOL result = NO;
    if (participantHandle->IsUsingTts) {
        result = YES;
    }
    return result;
}

- (BOOL)isHost
{
    BOOL result = NO;
    if (participantHandle->IsHost) {
        result = YES;
    }
    return result;
}

@end
