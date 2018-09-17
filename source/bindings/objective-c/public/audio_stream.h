//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef audio_stream_h
#define audio_stream_h

#import <Foundation/Foundation.h>
#import "audio_stream_format.h"

@class PullAudioInputStream;
@class PushAudioInputStream;

/**
  * Represents audio input stream used for custom audio input configurations.
  */
@interface AudioInputStream : NSObject

typedef NSInteger (^PullAudioInputStreamReadBlock)(NSMutableData *, NSUInteger);
typedef void (^PullAudioInputStreamCloseBlock)(void);
typedef NSInteger (^PullAudioInputStreamReadBlockWithContext)(void *, NSMutableData *, NSUInteger);
typedef void (^PullAudioInputStreamCloseBlockWithContext)(void *);

/**
  * Creates a memory backed PushAudioInputStream using the default format (16Khz 16bit mono PCM).
  * @return The audio input stream being created.
  */
+ (PushAudioInputStream *)createPushStream;

/**
  * Creates a memory backed PushAudioInputStream with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @return The audio input stream being created.
  */
+ (PushAudioInputStream *)createPushStreamWithFormat:(AudioStreamFormat *)format;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, using the default format (16Khz 16bit mono PCM).
  * @param userContext pointer to user-provided context data that will be passed to the caller as parameter of Read() or Close() block.
  * @param readBlock a block which will be called in order to read data from the audio input stream. The userContext is passed as a parameter. If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream. The userContext is passed as a parameter.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)createPullStreamWithContext:
(void *)userContext readBlock:(PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, using the default format (16Khz 16bit mono PCM).
  * @param readBlock a block which will be called in order to read data from the audio input stream.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)createPullStreamUsingReadBlock:(PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @param userContext pointer to user-provided context data that will be passed to the caller as parameter of Read() or Close() block.
  * @param readBlock a block which will be called in order to read data from the audio input stream. The userContext is passed as a parameter.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream. The userContext is passed as a parameter.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)createPullStreamUsingAudioFormat:(AudioStreamFormat *)format withContext:(void *)userContext readBlock: (PullAudioInputStreamReadBlockWithContext)readBlock closeBlock:(PullAudioInputStreamCloseBlockWithContext)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @param readBlock a block which will be called in order to read data from the audio input stream.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)createPullStreamUsingAudioFormat:(AudioStreamFormat *)format readBlock:(PullAudioInputStreamReadBlock)readBlock closeBlock:(PullAudioInputStreamCloseBlock)closeBlock;

@end

/**
  * Represents memory backed push audio input stream used for custom audio input configurations.
  */
@interface PushAudioInputStream : AudioInputStream

/**
  * Creates a memory backed PushAudioInputStream using the default format (16Khz 16bit mono PCM).
  * @return The push audio input stream being created.
  */
+ (PushAudioInputStream *)create;

/**
  * Creates a memory backed PushAudioInputStream with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @return The push audio input stream being created.
  */
+ (PushAudioInputStream *)createUsingAudioFormat: (AudioStreamFormat *)format;

/**
  * Writes the audio data specified by making an internal copy of the data.
  * @param data The audio buffer of which this function will make a copy.
  */
- (void)write: (NSData *)data;

/**
  * Closes the stream.
  */
- (void)close;

@end

/**
  * Represents audio input stream used for custom audio input configurations.
  */
@interface PullAudioInputStream : AudioInputStream

/**
  * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
  * @param userContext pointer to user-provided context data that will be passed to the caller as parameter of Read() or Close() block.
  * @param readBlock a block which will be called in order to read data from the audio input stream. The userContext is passed as a parameter.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream. The userContext is passed as a parameter.
  * @return The push audio input stream being created.
  */
+ (PullAudioInputStream *)CreateWithContext:(void *)userContext readBlock:(PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16Khz 16bit mono PCM).
  * @param readBlock a block which will be called in order to read data from the audio input stream.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream.
  * @return The push audio input stream being created.
  */
+ (PullAudioInputStream *)CreateUsingReadBlock:(PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @param userContext pointer to user-provided context data that will be passed to the caller as parameter of Read() or Close() block.
  * @param readBlock a block which will be called in order to read data from the audio input stream. The userContext is passed as a parameter.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream. The userContext is passed as a parameter.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)CreateUsingAudioFormat:(AudioStreamFormat *)format withContext:(void *)userContext readBlock: (PullAudioInputStreamReadBlockWithContext)readBlock closeBlock: (PullAudioInputStreamCloseBlockWithContext)closeBlock;

/**
  * Creates a PullAudioInputStream that delegates to the specified callback functions for Read() and Close() methods, with the specified audio format.
  * @param format The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16Khz 16bit mono PCM).
  * @param readBlock a block which will be called in order to read data from the audio input stream.  If no data is currently available in the stream, the readBlock should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream is closed.
  * @param closeBlock a block which will be called in order to close the audio input stream.
  * @return The audio input stream being created.
  */
+ (PullAudioInputStream *)CreateUsingAudioFormat:(AudioStreamFormat *)format readBlock:(PullAudioInputStreamReadBlock)readBlock closeBlock: (PullAudioInputStreamCloseBlock)closeBlock;

@end

#endif /* audio_stream_h */
