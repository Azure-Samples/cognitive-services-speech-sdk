//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXUser
{
    UserSharedPtr userHandle;
}

- (instancetype)initFromUserId:(NSString *)userId
{
     try {
        auto userImpl = TranscriptionImpl::User::FromUserId([userId toSpxString]);
        if (userImpl == nullptr)
            return nil;
        return [self initWithImpl:userImpl];
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
        NSLog(@"Exception caught when creating SPXUser in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initFromUserId:(NSString *)userId error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initFromUserId:userId];
        return self;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return nil;
}

- (instancetype)initWithImpl:(UserSharedPtr)userImpl
{
    self = [super init];
    self->userHandle = userImpl;
    if (!self || self->userHandle == nullptr) {
        NSLog(@"userHandle is nil in SPXUser constructor");
        return nil;
    }
    else
    {
        return self;
    }
}

- (UserSharedPtr)getHandle
{
    return userHandle;
}

- (instancetype)initWithError: (NSString *)message
{
    self->userHandle = nullptr;
    return self;
}

- (void)dealloc 
{
    LogDebug(@"user object deallocated.");
    if (!self->userHandle) {
        NSLog(@"userHandle is nil in SPXUser destructor");
        return;
    }

    try {
        userHandle.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in SPXUser destructor");
    }
}

- (NSString *)userId
{
    return [NSString StringWithStdString:userHandle->GetId()];
}

@end
