//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioStreamFormat.h"
#import "SPXSpeechEnums.h"

typedef NSInteger (^SPXPullAudioInputStreamReadHandler)(NSMutableData * _Nonnull, NSUInteger);
typedef void (^SPXPullAudioInputStreamCloseHandler)(void);
typedef NSString * _Nonnull (^SPXPullAudioInputStreamGetPropertyHandler)(SPXPropertyId);

typedef NSUInteger (^SPXPushAudioOutputStreamWriteHandler)(NSData * _Nonnull);
typedef void (^SPXPushAudioOutputStreamCloseHandler)(void);


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
 * Initializes a memory-backed SPXPushAudioInputStream using the default format (16 kHz, 16 bit, mono PCM).
 * @return an instance of push audio input stream.
 */
- (nonnull instancetype)init;

/**
 * Initializes a memory-backed SPXPushAudioInputStream with the specified audio format.
 *
 * @param format The audio data format in which audio will be written to the push audio stream's write() method.
 * @return an instance of push audio input stream.
 */
- (nullable instancetype)initWithAudioFormat:(nonnull SPXAudioStreamFormat *)format;

/**
 * Writes the specified audio data by making an internal copy of the data.
 * Note: The data should not contain any audio header.
 *
 * @param data The audio buffer of which this function will make a copy.
 */
- (void)write:(nonnull NSData *)data;

/**
 * Closes the stream.
 */
- (void)close;

@end

/**
 * Represents an audio input stream used for custom audio input configurations.
 */
SPX_EXPORT
@interface SPXPullAudioInputStream : SPXAudioInputStream

/**
 * Initializes an SPXPullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
 *
 * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when the stream should be closed. The data returned by read() should not contain any audio header.
 * @param closeHandler handler which will be called in order to close the audio input stream.
 * @return an instance of pull audio input stream.
 */
- (nullable instancetype)initWithReadHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

/**
 * Initializes an SPXPullAudioInputStream that delegates to the specified callback functions for read() and close() methods, with the specified audio format.
 *
 * @param format The audio data format in which audio will be written to the push audio stream's write() method.
 * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream should be closed. The data returned by read() should not contain any audio header.
 * @param closeHandler handler which will be called in order to close the audio input stream.
 * @return The audio input stream being created.
 */
- (nullable instancetype)initWithAudioFormat:(nonnull SPXAudioStreamFormat *)format readHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

/**
 * Initializes an SPXPullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz, 16 bit, mono PCM).
 *
 * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when the stream should be closed. The data returned by read() should not contain any audio header.
 * @param getPropertyHandler handler which will be called in order to get string information associated to given SPXPropertyId.
 * @param closeHandler handler which will be called in order to close the audio input stream.
 * @return an instance of pull audio input stream.
 */
- (nullable instancetype)initWithReadHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler getPropertyHandler:(nonnull SPXPullAudioInputStreamGetPropertyHandler)getPropertyHandler closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

/**
 * Initializes an SPXPullAudioInputStream that delegates to the specified callback functions for read() and close() methods, with the specified audio format.
 *
 * @param format The audio data format in which audio will be written to the push audio stream's write() method.
 * @param readHandler handler which will be called in order to read data from the audio input stream. If no data is currently available in the stream, the readHandler should wait until data is available. It returns the number of bytes that have been read. It returns 0 when stream should be closed. The data returned by read() should not contain any audio header.
 * @param getPropertyHandler handler which will be called in order to get string information associated to given SPXPropertyId.
 * @param closeHandler handler which will be called in order to close the audio input stream.
 * @return The audio input stream being created.
 */

- (nullable instancetype)initWithAudioFormat:(nonnull SPXAudioStreamFormat *)format readHandler:(nonnull SPXPullAudioInputStreamReadHandler)readHandler
    getPropertyHandler:(nonnull SPXPullAudioInputStreamGetPropertyHandler)getPropertyHandler
        closeHandler:(nonnull SPXPullAudioInputStreamCloseHandler)closeHandler;

@end

/**
 * Base class representing the audio output stream used for custom audio output configurations.
 * Currently no property and methods defined in the base class.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXAudioOutputStream : NSObject

@end

/**
 * Represents memory backed pull audio output stream used for custom audio output.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXPullAudioOutputStream : SPXAudioOutputStream

/**
 * Creates a memory backed PullAudioOutputStream with the specified audio format.
 * 
 * @return An instance of pull audio output stream
 */
- (nonnull instancetype)init;

/**
 * Reads a chunk of the audio data and fill it to given buffer
 * 
 * @param data a buffer to receive read data.
 * @param length the length of data to receive.
 * @return size of data filled to the buffer, 0 means end of stream
 */
- (NSUInteger)read:(nonnull NSMutableData *)data length:(NSUInteger) length;

@end

/**
 * Represents a push audio output stream used for custom audio output configurations.
 * 
 * Added in version 1.7.0
 */
SPX_EXPORT
@interface SPXPushAudioOutputStream : SPXAudioOutputStream

/**
 * Initializes an SPXPushAudioOutputStream that delegates to the specified callback interface for write() and close() methods.
 *
 * @param writeHandler Handler which will be called in order to write data synchronously to the data stream.
 * @param closeHandler Handler which will be called to close the audio stream.
 * 
 * @return an instance of push audio output stream.
 */
- (nullable instancetype)initWithWriteHandler:(nonnull SPXPushAudioOutputStreamWriteHandler)writeHandler closeHandler:(nonnull SPXPushAudioOutputStreamCloseHandler)closeHandler; 

@end
