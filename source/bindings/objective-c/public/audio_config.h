//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_config_h
#define audio_config_h

#import <Foundation/Foundation.h>
#import "audio_stream.h"

/**
  * Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
  */
@interface AudioConfig : NSObject

/**
  * Creates an AudioConfig object representing the default microphone on the system.
  * @return audio input configuration being created.
  */
+ (AudioConfig *)fromDefaultMicrophoneInput;

/**
  * Creates an AudioConfig object representing the specified file.
  * @param path path of the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
  * @return audio input configuration being created.
  */
+ (AudioConfig *)fromWavFileInput:(NSString *)path;

/**
  * Creates an AudioConfig object representing the specified stream.
  * @param stream the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
  * @return audio input configuration being created.
*/
+ (AudioConfig *)fromStreamInput:(AudioInputStream *)stream;

@end

#endif /* audio_config_h */
