//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
  * Represents the audio stream format used for custom audio input configurations.
  */
SPX_EXPORT
@interface SPXAudioStreamFormat : NSObject

/**
  * Initializes the audio stream format object with the default audio stream format (16 kHz 16bit mono PCM).
  * @return an instance of audio stream format.
  */
- (nullable instancetype)init;

/**
  * Initialize the audio stream format object with the specified PCM waveformat characteristics.
  * @param samplesPerSecond sample rate, in samples per second (Hertz).
  * @param bitsPerSample bits per sample, typically 16.
  * @param channels number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.
  * @return an instance of audio stream format.
  */
- (nullable instancetype)initUsingPCMWithSampleRate:(NSUInteger)samplesPerSecond bitsPerSample:(NSUInteger)bitsPerSample channels:(NSUInteger)channels;

@end
