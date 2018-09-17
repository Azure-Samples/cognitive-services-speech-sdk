//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_stream_format_h
#define audio_stream_format_h

#import <Foundation/Foundation.h>

/**
  * Represents audio stream format used for custom audio input configurations.
  */
@interface AudioStreamFormat : NSObject

/**
  * Creates an audio stream format object representing the default audio stream format (16Khz 16bit mono PCM).
  * @return The audio stream format being created.
  */
+ (AudioStreamFormat *)getDefaultInputFormat;

/**
  * Creates an audio stream format object with the specified PCM waveformat characteristics.
  * @param samplesPerSecond sample rate, in samples per second (hertz).
  * @param bitsPerSample bits per sample, typically 16.
  * @param channels number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.
  * @return The audio stream format being created.
  */
+ (AudioStreamFormat *)getWaveFormatPCMWithSampleRate:(NSUInteger)samplesPerSecond bitsPerSample:(NSUInteger)bitsPerSample channels:(NSUInteger)channels;

@end

#endif /* audio_stream_format_h */
