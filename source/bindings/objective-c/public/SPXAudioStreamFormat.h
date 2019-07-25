//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioStreamFormat.h"

/**
  * Defines supported audio stream container format.
  * Added in version 1.7.0.
  * Note: Compressed input is only supported on iOS.
  */
typedef NS_ENUM(NSUInteger, SPXAudioStreamContainerFormat)
{
    /**
      * Stream ContainerFormat definition for OGG OPUS.
      */
    SPXAudioStreamContainerFormat_OGG_OPUS = 0x101,

    /**
      * Stream ContainerFormat definition for MP3.
      */
    SPXAudioStreamContainerFormat_MP3 = 0x102,

    /**
      * Stream ContainerFormat definition for FLAC. Not supported yet.
      */
    SPXAudioStreamContainerFormat_FLAC = 0x103
};

/**
 * Represents the audio stream format used for custom audio input configurations.
 */
SPX_EXPORT
@interface SPXAudioStreamFormat : NSObject

/**
 * Initializes the audio stream format object with the default audio stream format (16 kHz, 16 bit, mono PCM).
 *
 * @return an instance of audio stream format.
 */
- (nullable instancetype)init;

/**
 * Initialize the audio stream format object with the specified PCM waveformat characteristics.
 * Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
 *
 * @param samplesPerSecond sample rate, in samples per second (Hertz).
 * @param bitsPerSample bits per sample.
 * @param channels number of channels in the waveform-audio data.
 * @return an instance of audio stream format.
 */
- (nullable instancetype)initUsingPCMWithSampleRate:(NSUInteger)samplesPerSecond bitsPerSample:(NSUInteger)bitsPerSample channels:(NSUInteger)channels;

/**
  * Creates an audio stream format object with the specified compressed audio container format, to be used as input format.
  * Support added in 1.5.0.
  * Note: Compressed input is only supported on iOS.
  * @param audioStreamContainerFormat compressed format type.
  * @return an instance of audio stream format.
  */
- (nullable instancetype)initUsingCompressedFormat:(SPXAudioStreamContainerFormat)audioStreamContainerFormat;
@end

