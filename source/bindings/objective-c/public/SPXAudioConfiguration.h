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
 *
 * @return an instance of audio input configuration.
 */
- (nonnull instancetype)init;

/**
 * Initializes an SPXAudioConfiguration object using the specified audio input of the system.
 *
 * Added in version 1.3.0.
 *
 * @param deviceName the unique ID of the input device to be used. If this is nil, the default is used.  Please refer to <a href="https://aka.ms/csspeech/microphone-selection">this page</a> on how to retrieve platform-specific microphone names.
 * @note Specifying a non-default device name is not supported in iOS.
 * @note If the specified device is not available, a failure will occur when starting recognition using this SPXAudioConfiguration object.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithMicrophone:(nullable NSString *)deviceName;

/**
 * Initializes an SPXAudioConfiguration object using the specified file as input.
 * Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
 *
 * @param path path of the audio input file.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithWavFileInput:(nonnull NSString *)path;

/**
 * Initializes an SPXAudioConfiguration object using the default audio output device (speaker) on the system as output.
 * Please ensure that the speaker is available before using this method.
 *
 * Added in version 1.7.0
 *
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithDefaultSpeakerOutput
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXAudioConfiguration object using the default audio output device (speaker) on the system as output.
 * Please ensure that the speaker is available before using this method.
 *
 * Added in version 1.7.0
 *
 * @param outError error information.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithDefaultSpeakerOutput:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an SPXAudioConfiguration object using the specified file as output.
 *
 * Added in version 1.7.0
 *
 * @param path path of the audio output file. The parent directory must already exist.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithWavFileOutput:(nonnull NSString *)path
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXAudioConfiguration object using the specified file as output.
 *
 * Added in version 1.7.0
 *
 * @param path path of the audio output file. The parent directory must already exist.
 * @param outError error information.
 * @return an instance of audio input configuration.
 */
- (nullable instancetype)initWithWavFileOutput:(nonnull NSString *)path error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an SPXAudioConfiguration object using the specified stream as input.
 * Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
 *
 * @param stream the custom audio input stream.
 * @return an instance of audio input configuration.
*/
- (nullable instancetype)initWithStreamInput:(nonnull SPXAudioInputStream *)stream;

/**
 * Initializes an SPXAudioConfiguration object using the specified stream as output.
 *
 * Added in version 1.7.0
 *
 * @param stream the custom audio output stream.
 * @return an instance of audio output configuration.
*/
- (nullable instancetype)initWithStreamOutput:(nonnull SPXAudioOutputStream *)stream
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an SPXAudioConfiguration object using the specified stream as output.
 *
 * Added in version 1.7.0
 *
 * @param stream the custom audio output stream.
 * @param outError error information.
 * @return an instance of audio output configuration.
*/
- (nullable instancetype)initWithStreamOutput:(nonnull SPXAudioOutputStream *)stream error:(NSError * _Nullable * _Nullable)outError;



@end
