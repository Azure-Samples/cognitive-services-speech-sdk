//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_stream_private.h"
#import "audio_stream_format_private.h"
#import "speechapi_private.h"

@implementation SPXAudioInputStream
{
    std::shared_ptr<AudioImpl::AudioInputStream> inputStreamImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioInputStream>)handle
{
    self = [super init];
    inputStreamImpl = handle;
    return self;
}

- (std::shared_ptr<AudioImpl::AudioInputStream>)getHandle
{
    return inputStreamImpl;
}

@end

@implementation SPXPushAudioInputStream
{
    std::shared_ptr<AudioImpl::PushAudioInputStream> pushInputStreamImpl;
}

- (instancetype)init
{
    auto pushImpl = AudioImpl::PushAudioInputStream::Create();
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pushImpl];
}

- (instancetype)initWithAudioFormat:(SPXAudioStreamFormat *)format
{
    auto pushImpl = AudioImpl::PushAudioInputStream::Create([format getHandle]);
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pushImpl];
}

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PushAudioInputStream>)handle
{
    self = [super init:handle];
    if (self) {
        self->pushInputStreamImpl = handle;
    }
    return self;
}

- (void)write:(NSData *)data
{
    pushInputStreamImpl->Write((uint8_t *)[data bytes], (uint32_t)[data length]);
}

- (void)close
{
    pushInputStreamImpl->Close();
}

@end

@implementation SPXPullAudioInputStream
{
    std::shared_ptr<AudioImpl::PullAudioInputStream> pullInputStreamImpl;
}

- (instancetype)initWithReadHandler:(SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(SPXPullAudioInputStreamCloseHandler)closeHandler
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readHandler(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    LogDebug(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeHandler();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pullImpl];
}

- (instancetype)initWithReadHandler:(SPXPullAudioInputStreamReadHandler)readHandler getPropertyHandler:(SPXPullAudioInputStreamGetPropertyHandler)getPropertyHandler closeHandler:(SPXPullAudioInputStreamCloseHandler)closeHandler
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readHandler(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    LogDebug(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
        closeHandler();
        },
        [=](SpeechImpl::PropertyId propertyId) -> std::string {
            NSString * result = getPropertyHandler((SPXPropertyId) propertyId);
            return [result toSpxString];
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pullImpl];
}

- (instancetype)initWithAudioFormat:(SPXAudioStreamFormat *)format readHandler:(SPXPullAudioInputStreamReadHandler)readHandler  closeHandler:(SPXPullAudioInputStreamCloseHandler)closeHandler
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readHandler(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    LogDebug(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeHandler();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pullImpl];
}

- (instancetype)initWithAudioFormat:(SPXAudioStreamFormat *)format readHandler:(SPXPullAudioInputStreamReadHandler)readHandler getPropertyHandler:(SPXPullAudioInputStreamGetPropertyHandler)getPropertyHandler closeHandler:(SPXPullAudioInputStreamCloseHandler)closeHandler
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readHandler(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    LogDebug(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeHandler();
        },
        [=](SpeechImpl::PropertyId propertyId) -> std::string {
            NSString * result = getPropertyHandler((SPXPropertyId) propertyId);
            return [result toSpxString];
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pullImpl];
}

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PullAudioInputStream>)handle
{
    self = [super init:handle];
    if (self) {
        self->pullInputStreamImpl = handle;
    }
    return self;
}

@end

@implementation SPXAudioOutputStream
{
    std::shared_ptr<AudioImpl::AudioOutputStream> outputStreamImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioOutputStream>)handle
{
    self = [super init];
    outputStreamImpl = handle;
    return self;
}

- (std::shared_ptr<AudioImpl::AudioOutputStream>)getHandle
{
    return outputStreamImpl;
}

@end

@implementation SPXPullAudioOutputStream
{
    std::shared_ptr<AudioImpl::PullAudioOutputStream> pullOutputStreamImpl;
}

- (instancetype)init
{
    auto pullImpl = AudioImpl::PullAudioOutputStream::Create();
    if (pullImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pullImpl];
}

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PullAudioOutputStream>)handle
{
    self = [super init:handle];
    if (self) {
        self->pullOutputStreamImpl = handle;
    }
    return self;
}

- (NSUInteger)read:(NSMutableData *)data length:(NSUInteger) length
{
    uint8_t* buffer = new uint8_t[length];
    NSUInteger ret = (NSUInteger)pullOutputStreamImpl->Read(buffer, (uint32_t)length);
    [data replaceBytesInRange:NSMakeRange(0, [data length]) withBytes:buffer length:ret];
    return ret;
}

@end

@implementation SPXPushAudioOutputStream
{
    std::shared_ptr<AudioImpl::PushAudioOutputStream> pushOutputStreamImpl;
}

- (instancetype)initWithWriteHandler:(SPXPushAudioOutputStreamWriteHandler)writeHandler closeHandler:(SPXPushAudioOutputStreamCloseHandler)closeHandler
{
    auto pushImpl = AudioImpl::PushAudioOutputStream::Create(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSData* data = [[NSData alloc] initWithBytesNoCopy:(void*)buffer length:(NSUInteger)size freeWhenDone:false]; // no copy. do not take ownership.
            NSUInteger ret = writeHandler(data);
            if (ret > 0) {
                if (ret > [data length]) {
                    LogDebug(@"The length of data is less than return value.");
                    ret = [data length];
                }
            }
            return (int)ret;
        },
        [=]() -> void {
            closeHandler();
        });

    if (pushImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    return [self initWithImpl:pushImpl];
}

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PushAudioOutputStream>)handle
{
    self = [super init:handle];
    if (self) {
        self->pushOutputStreamImpl = handle;
    }
    return self;
}

@end
