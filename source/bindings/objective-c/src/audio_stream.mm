//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_stream_private.h"
#import "audio_stream_format_private.h"
#import "common_private.h"

@implementation AudioInputStream
{
    std::shared_ptr<AudioImpl::AudioInputStream> streamImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioInputStream>)handle
{
    self = [super init];
    streamImpl = handle;
    return self;
}

- (std::shared_ptr<AudioImpl::AudioInputStream>)getHandle
{
    return streamImpl;
}

+ (PushAudioInputStream *)createPushStream
{
    auto pushImpl = AudioImpl::AudioInputStream::CreatePushStream();
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    else 
        return [[PushAudioInputStream alloc] init :pushImpl];
}

+ (PushAudioInputStream *)createPushStreamWithFormat: (AudioStreamFormat *)format
{
    auto pushImpl = AudioImpl::AudioInputStream::CreatePushStream([format getHandle]);
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    else 
        return [[PushAudioInputStream alloc] init :pushImpl];
}

// Todo: provide protocol based interface for pull stream
+ (PullAudioInputStream *)createPullStreamWithContext:
(void *)userContext readBlock: (PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock
{
    auto pullImpl = AudioImpl::AudioInputStream::CreatePullStream(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(userContext, data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock(userContext);
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)createPullStreamUsingReadBlock: (PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock
{
    auto pullImpl = AudioImpl::AudioInputStream::CreatePullStream(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)createPullStreamUsingAudioFormat: (AudioStreamFormat *)format withContext: (void *)userContext readBlock: (PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock
{
    auto pullImpl = AudioImpl::AudioInputStream::CreatePullStream(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(userContext, data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock(userContext);
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)createPullStreamUsingAudioFormat: (AudioStreamFormat *)format readBlock: (PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock
{
    auto pullImpl = AudioImpl::AudioInputStream::CreatePullStream(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

@end

@implementation PushAudioInputStream
{
    std::shared_ptr<AudioImpl::PushAudioInputStream> pushImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::PushAudioInputStream>)handle
{
    self = [super init :handle];
    pushImpl = handle;
    return self;
}

+ (PushAudioInputStream *)create
{
    auto pushImpl = AudioImpl::PushAudioInputStream::Create();
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    else 
        return [[PushAudioInputStream alloc] init :pushImpl];
}

+ (PushAudioInputStream *)createUsingAudioFormat: (AudioStreamFormat *)format
{
    auto pushImpl = AudioImpl::PushAudioInputStream::Create([format getHandle]);
    if (pushImpl == nullptr) {
        NSLog(@"Unable to create push audio input stream in core");
        return nil;
    }
    else 
        return [[PushAudioInputStream alloc] init :pushImpl];
}

- (void)write: (NSData *)data
{
    pushImpl->Write((uint8_t *)[data bytes], (uint32_t)[data length]);
}

- (void)close
{
    pushImpl->Close();
}

@end

@implementation PullAudioInputStream
{
    std::shared_ptr<AudioImpl::PullAudioInputStream> pullImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::PullAudioInputStream>)handle
{
    self = [super init :handle];
    pullImpl = handle;
    return self;
}

+ (PullAudioInputStream *)CreateWithContext: (void *)userContext readBlock:(PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(userContext, data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock(userContext);
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)CreateUsingReadBlock: (PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock) closeBlock
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)CreateUsingAudioFormat: (AudioStreamFormat *)format withContext: (void *)userContext readBlock: (PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(userContext, data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock(userContext);
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

+ (PullAudioInputStream *)CreateUsingAudioFormat: (AudioStreamFormat *)format readBlock: (PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock
{
    auto pullImpl = AudioImpl::PullAudioInputStream::Create(
        [format getHandle],
        [=](uint8_t* buffer, uint32_t size) -> int {
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity: size];
            NSInteger ret = readBlock(data, (NSUInteger)size);
            if (ret > 0) {
                if (ret > [data length]) {
                    NSLog(@"The length of data is less than return value.");
                    ret = [data length];
                }
                std::memcpy(buffer, [data bytes], ret);
            }
            return (int)ret;
        },
        [=]() -> void {
            closeBlock();
        });

    if (pullImpl == nullptr) {
        NSLog(@"Unable to create pull audio input stream in core");
        return nil;
    }
    else 
        return [[PullAudioInputStream alloc] init :pullImpl];
}

@end
