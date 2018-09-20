//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "audio_stream_format_private.h"
#import "common_private.h"

@implementation SPXAudioStreamFormat
{
    std::shared_ptr<AudioImpl::AudioStreamFormat> audioStreamFormatImpl;
}

- (instancetype)init
{
    self = [super init];
    auto impl = AudioImpl::AudioStreamFormat::GetDefaultInputFormat();
    if (impl == nullptr) {
        NSLog(@"Unable to create audio stream format in core");
        return nil;
    }
    return self;
}

- (instancetype)initUsingPCMWithSampleRate:(NSUInteger)samplesPerSecond bitsPerSample:(NSUInteger)bitsPerSample channels:(NSUInteger)channels
{
    self = [super init];
    //Todo: check size convertion error.
    auto impl = AudioImpl::AudioStreamFormat::GetWaveFormatPCM((uint32_t)samplesPerSecond, (uint8_t)bitsPerSample, (uint8_t)channels);
    if (impl == nullptr) {
        NSLog(@"Unable to create audio stream format in core");
        return nil;
    }
    return self;
}

- (std::shared_ptr<AudioImpl::AudioStreamFormat>)getHandle
{
    return audioStreamFormatImpl;
}

@end
