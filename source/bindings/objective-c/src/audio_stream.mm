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

- (instancetype)initWithAudioFormat:(SPXAudioStreamFormat *)format readHandler:(SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(SPXPullAudioInputStreamCloseHandler)closeHandler
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

- (instancetype)initWithImpl: (std::shared_ptr<AudioImpl::PullAudioInputStream>)handle
{
    self = [super init:handle];
    if (self) {
        self->pullInputStreamImpl = handle;
    }
    return self;
}

@end
