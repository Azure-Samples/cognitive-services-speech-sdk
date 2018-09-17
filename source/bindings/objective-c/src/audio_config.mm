//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_config_private.h"
#import "audio_stream_private.h"
#import "common_private.h"

@implementation AudioConfig
{
    std::shared_ptr<AudioImpl::AudioConfig> audioImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioConfig>)audioHandle
{
    self = [super init];
    audioImpl = audioHandle;
    return self;
}

- (std::shared_ptr<AudioImpl::AudioConfig>)getHandle
{
    return audioImpl;
}

+ (AudioConfig *)fromDefaultMicrophoneInput
{
    auto audioImpl = AudioImpl::AudioConfig::FromDefaultMicrophoneInput();
    if (audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core");
        return nil;
    }
    else 
        return [[AudioConfig alloc] init :audioImpl];
}

+ (AudioConfig *)fromWavFileInput: (NSString *)path
{
    auto audioImpl = AudioImpl::AudioConfig::FromWavFileInput([path string]);
    if (audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core");
        return nil;
    }
    else 
        return [[AudioConfig alloc] init :audioImpl];
}

+ (AudioConfig *)fromStreamInput: (AudioInputStream *)stream
{
    auto audioImpl = AudioImpl::AudioConfig::FromStreamInput([stream getHandle]);
    if (audioImpl == nullptr) {
        NSLog(@"Unable to create audio config in core");
        return nil;
    }
    else
        return [[AudioConfig alloc] init :audioImpl];
}

@end
