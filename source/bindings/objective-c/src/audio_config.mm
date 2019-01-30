//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_config_private.h"
#import "audio_stream_private.h"
#import "common_private.h"

@implementation SPXAudioConfiguration
{
    std::shared_ptr<AudioImpl::AudioConfig> audioImpl;
}

- (instancetype)init
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromDefaultMicrophoneInput();
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using default microphone.");
        return nil;
    }
    return self;
}

- (instancetype)initWithMicrophone:(NSString *)deviceName
{
    self = [super init];
    if (nil == deviceName) {
        return [self init];
    }

    self->audioImpl = AudioImpl::AudioConfig::FromMicrophoneInput([deviceName string]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using specified microphone.");
        return nil;
    }
    return self;
}

- (instancetype)initWithWavFileInput:(NSString *)path
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromWavFileInput([path string]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using wav file input.");
        return nil;
    }
    return self;
}

- (instancetype)initWithStreamInput:(SPXAudioInputStream *)stream
{
    self = [super init];
    self->audioImpl = AudioImpl::AudioConfig::FromStreamInput([stream getHandle]);
    if (self->audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core using stream input.");
        return nil;
    }
    return self;
}

- (std::shared_ptr<AudioImpl::AudioConfig>)getHandle
{
    return audioImpl;
}

@end
