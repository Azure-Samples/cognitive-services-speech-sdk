//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechConfiguration.h"
#import "SPXAudioConfiguration.h"
#import "SPXPropertyCollection.h"
#import "SPXRecognizer.h"
#import "SPXConversation.h"
#import "SPXConversationTranscriptionResult.h"
#import "SPXConversationTranscriptionEventArgs.h"

/**
 * Performs conversation transcribing for audio input streams, and gets transcribed text and user id as a result.
 */
SPX_EXPORT
@interface SPXConversationTranscriber : SPXRecognizer

typedef void (^SPXConversationTranscriptionEventHandler)(SPXConversationTranscriber * _Nonnull, SPXConversationTranscriptionEventArgs * _Nonnull);
typedef void (^SPXConversationTranscriptionCanceledEventHandler)(SPXConversationTranscriber * _Nonnull, SPXConversationTranscriptionCanceledEventArgs * _Nonnull);

/**
 * The authorization token used to communicate with the conversation transcription service.
 * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
 * the caller needs to refresh it by calling this setter with a new valid token.
 * Otherwise, the recognizer will encounter errors during recognition.
 */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
 * Initializes a new instance of a conversation transcriber using the default configuration.
 * @return a conversation transcriber instance.
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability"
- (nullable instancetype)init
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");
#pragma clang diagnostic pop
/**
 * Initializes a new instance of a conversation transcriber using the default configuration.
 *
 * @param outError error information.
 * @return a conversation transcriber instance.
 */
- (nullable instancetype)init:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes a new instance of a conversation transcriber using the specified audio configuration.
 *
 * @param audioConfiguration audio configuration.
 * @return a conversation transcriber instance.
 */
- (nullable instancetype)initWithAudioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of a conversation transcriber using the specified audio configuration.
 *
 * @param audioConfiguration audio configuration.
 * @return a conversation transcriber instance.
 */
- (nullable instancetype)initWithAudioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Join a conversation.
 * A conversation transcriber must join a conversation before transcribing audio.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversation speech configuration.
 */
- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Join a conversation.
 * A conversation transcriber must join a conversation before transcribing audio.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversation speech configuration.
 * @param outError error information.
 */
- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation error:(NSError * _Nullable * _Nullable)outError;

/**
 * Leave a conversation.
 * After leaving a conversation, no transcribing and transcribed events will be sent out.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Leave a conversation.
 * After leaving a conversation, no transcribing and transcribed events will be sent out.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Starts conversation transcribing on a continuous audio stream, until stopTranscribingAsync() is called.
 * User must subscribe to events to receive transcription results.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Starts conversation transcribing on a continuous audio stream, until stopTranscribingAsync() is called.
 * User must subscribe to events to receive transcription results.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Stops conversation transcribing.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Stops conversation transcribing.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Subscribes to the Transcribed event which indicates that a final result has been transcribed.
 */
- (void)addTranscribedEventHandler:(nonnull SPXConversationTranscriptionEventHandler)eventHandler;

/**
 * Subscribes to the Transcribing event which indicates an that intermediate result has been transcribed.
 */
- (void)addTranscribingEventHandler:(nonnull SPXConversationTranscriptionEventHandler)eventHandler;

/**
 * Subscribes to the Canceled event which indicates that an error occurred during transcription.
 */
- (void)addCanceledEventHandler:(nonnull SPXConversationTranscriptionCanceledEventHandler)eventHandler;

@end
