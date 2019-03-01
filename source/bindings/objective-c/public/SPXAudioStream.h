//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioStreamFormat.h"

typedef NSInteger (^SPXPullAudioInputStreamReadHandler)(NSMutableData * _Nonnull, NSUInteger);
typedef void (^SPXPullAudioInputStreamCloseHandler)(void);

/**
 * Base class representing the audio input stream used for custom audio input configurations.
 * Currently no property and methods defined in the base class.
 */
SPX_EXPORT
@interface SPXAudioInputStream : NSObject

@end

/**
  * Represents memory backed push audio input stream used for custom audio input configurations.
  */
SPX_EXPORT
@interface SPXPushAudioInputStream : SPXAudioInputStream

/**
  * Initializes a memory-backed SPXPushAudioInputStream using the default format (16 kHz 16bit mono PCM).
  * @return an instance of push audio input stream.
  */
- (nullable instancetype)init;

/**
  * Initializes a memory-backed SPXPushAudioInputStream with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16 kHz 16bit mono PCM).
  * @return an instance of push audio input stream.
  */
- (nullable instancetype)initWithAudioFormat:(nonnull SPXAudioStreamFormat *)format;

/**
  * Writes the specified audio data by making an internal copy of the data.
  * Note: The data should not contain any audio header.
  * @param data The audio buffer of which this function will make a copy.
  */
- (void)write:(nonnull NSData *)data;

/**
  * Closes the stream.
  */
- (void)close;

@end

/**
  * Represents audio input stream used for custom audio input configurations.
  */
SPX_EXPORT
@interface SPXPullAudioInputStream : SPXAudioInputStream

/**
  * Initializes an SPXPullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz 16bit mono PCM).
  * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when the stream should be closed. The data returned by read() should not contain any audio header.
  * @param closeHandler handler which will be called in order to close the audio input stream.
  * @return an instance of pull audio input stream.
  */
- (nullable instancetype)initWithReadHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

/**
  * Initializes an SPXPullAudioInputStream that delegates to the specified callback functions for read() and close() methods, with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16 kHz 16bit mono PCM).
  * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream should be closed. The data returned by read() should not contain any audio header.
  * @param closeHandler handler which will be called in order to close the audio input stream.
  * @return The audio input stream being created.
  */
- (nullable instancetype)initWithAudioFormat:(nonnull SPXAudioStreamFormat *)format readHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

@end
