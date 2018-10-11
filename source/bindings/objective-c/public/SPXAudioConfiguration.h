//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioStream.h"

/**
  * Represents the audio input configuration used for specifying what type of input to use (microphone, file, stream).
  */
SPX_EXPORT
@interface SPXAudioConfiguration : NSObject

/**
  * Initializes an SPXAudioConfiguration object using the default microphone on the system.
  * @return an instance of audio input configuration.
  */
- (nullable instancetype)init;

/**
  * Initializes an SPXAudioConfiguration object using the specified file as input.
  * @param path path of the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
  * @return an instance of audio input configuration.
  */
- (nullable instancetype)initWithWavFileInput:(nonnull NSString *)path;

/**
  * Initializes an SPXAudioConfiguration object using the specified stream as input.
  * @param stream the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
  * @return an instance of audio input configuration.
*/
- (nullable instancetype)initWithStreamInput:(nonnull SPXAudioInputStream *)stream;

@end
