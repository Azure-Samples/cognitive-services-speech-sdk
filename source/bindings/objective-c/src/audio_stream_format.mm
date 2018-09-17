//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_stream_format_private.h"
#import "common_private.h"

@implementation AudioStreamFormat
{
    std::shared_ptr<AudioImpl::AudioStreamFormat> audioStreamFormatImpl;
}

- (instancetype)init: (std::shared_ptr<AudioImpl::AudioStreamFormat>)handle
{
    self = [super init];
    audioStreamFormatImpl = handle;
    return self;
}

- (std::shared_ptr<AudioImpl::AudioStreamFormat>)getHandle
{
    return audioStreamFormatImpl;
}

+ (AudioStreamFormat *)getDefaultInputFormat
{
    auto impl = AudioImpl::AudioStreamFormat::GetDefaultInputFormat();
    if (impl == nullptr) {
        NSLog(@"Unable to create audio stream format in core");
        return nil;
    }
    else 
        return [[AudioStreamFormat alloc] init :impl];
}

+ (AudioStreamFormat *)getWaveFormatPCMWithSampleRate: (NSUInteger)samplesPerSecond bitsPerSample: (NSUInteger)bitsPerSample channels: (NSUInteger)channels
{
    //Todo: check size convertion error.
    auto impl = AudioImpl::AudioStreamFormat::GetWaveFormatPCM((uint32_t)samplesPerSecond, (uint8_t)bitsPerSample, (uint8_t)channels);
    if (impl == nullptr) {
        NSLog(@"Unable to create audio stream format in core");
        return nil;
    }
    else 
        return [[AudioStreamFormat alloc] init :impl];
}

@end
