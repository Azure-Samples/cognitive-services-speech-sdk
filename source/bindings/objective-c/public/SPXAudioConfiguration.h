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
 * Initializes an SPXAudioConfiguration object using the specified audio input of the system.
 * NOTE: This functionality was added in version 1.3.0.
 * @param deviceName the unique ID of the input device to be used. If this is nil, the default is used.
   Please refer to <a href="https://aka.ms/csspeech/microphone-selection">this page</a> on how to retrieve platform-specific microphone names.
 * @note Specifying a non-default device name is not supported in iOS.
 * @note If the specified device is not available, a failure will occur when starting recognition using this SPXAudioConfiguration object.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithMicrophone:(nullable NSString *)deviceName;

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
