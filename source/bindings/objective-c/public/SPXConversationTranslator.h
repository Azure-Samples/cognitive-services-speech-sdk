//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechConfiguration.h"
#import "SPXAudioConfiguration.h"
#import "SPXPropertyCollection.h"
#import "SPXConversation.h"
#import "SPXConversationEventArgs.h"
#import "SPXConversationTranslationResult.h"
#import "SPXConversationTranslationEventArgs.h"

/**
 * A conversation translator that enables a connected experience where participants can use their
 * own devices to see everyone else's recognitions and IMs in their own languages. Participants can
 * also speak and send IMs to others.
 */
SPX_EXPORT
@interface SPXConversationTranslator : NSObject

typedef void (^SPXConversationTranslationEventHandler)(SPXConversationTranslator * _Nonnull, SPXConversationTranslationEventArgs * _Nonnull);
typedef void (^SPXConversationTranslationCanceledEventHandler)(SPXConversationTranslator * _Nonnull, SPXConversationTranslationCanceledEventArgs * _Nonnull);
typedef void (^SPXConversationParticipantsChangedEventHandler)(SPXConversationTranslator * _Nonnull, SPXConversationParticipantsChangedEventArgs * _Nonnull);
typedef void (^SPXConversationExpirationEventHandler)(SPXConversationTranslator * _Nonnull, SPXConversationExpirationEventArgs * _Nonnull);
typedef void (^SPXConversationSessionEventHandler)(SPXConversationTranslator * _Nonnull, SPXSessionEventArgs * _Nonnull);

/**
 * Authorization token used to communicate with the conversation translation service.
 *
 * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
 * the caller needs to refresh it by calling this setter with a new valid token.
 * Otherwise, the recognizer will encounter errors during recognition.
 */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
 * Participant identifier used for conversation translation.
 */
@property (nonatomic, copy, readonly, nullable)NSString *participantId;

/**
 * Initializes a new instance of a conversation translator using the default configuration.
 * @return a conversation translator instance.
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability"
- (nullable instancetype)init
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");
#pragma clang diagnostic pop

/**
 * Initializes a new instance of a conversation translator using the default configuration.
 *
 * @param outError error information.
 * @return a conversation translator instance.
 */
- (nullable instancetype)init:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes a new instance of a conversation translator using the specified audio configuration.
 *
 * @param audioConfiguration audio configuration.
 * @return a conversation translator instance.
 */
- (nullable instancetype)initWithAudioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of a conversation translator using the specified audio configuration.
 *
 * @param audioConfiguration audio configuration.
 * @return a conversation translator instance.
 */
- (nullable instancetype)initWithAudioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Joins an existing conversation. You should use this method if you have created a conversation
 * using createConversationAsync.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversation conversation instance.
 * @param nickname The display name to use for the current participant.
 */
- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation nickname:(nonnull NSString *)nickname
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Joins an existing conversation. You should use this method if you have created a conversation
 * using createConversationAsync.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversation conversation instance.
 * @param nickname The display name to use for the current participant.
 * @param outError error information.
 */
- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversation:(nonnull SPXConversation *)conversation nickname:(nonnull NSString *)nickname error:(NSError * _Nullable * _Nullable)outError;

/**
 * Joins an existing conversation. You should use this method if you have created a conversation
 * using createConversationAsync.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversationId conversation id.
 * @param nickname The display name to use for the current participant.
 * @param language The speech language to use for the current participant.
 */
- (void)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversationId:(nonnull NSString *)conversationId nickname:(nonnull NSString *)nickname language:(nonnull NSString *)language
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Joins an existing conversation. You should use this method if you have created a conversation
 * using createConversationAsync.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param conversationId conversation id.
 * @param nickname The display name to use for the current participant.
 * @param language The speech language to use for the current participant.
 * @param outError error information.
 */
- (BOOL)joinConversationAsync:(nonnull void (^)(BOOL joined, NSError * _Nullable))completedHandler conversationId:(nonnull NSString *)conversationId nickname:(nonnull NSString *)nickname language:(nonnull NSString *)language error:(NSError * _Nullable * _Nullable)outError;

/**
 * Leave the current conversation. After this is called, you will no longer receive any events.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Leave the current conversation. After this is called, you will no longer receive any events.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)leaveConversationAsync:(nonnull void (^)(BOOL left, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Sends an instant message to all participants in the conversation. This instant message will
 * be translated into each participant's text language.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param message The message to send.
 */
- (void)sendTextMessageAsync:(nonnull void (^)(BOOL sent, NSError * _Nullable))completedHandler message:(nonnull NSString *)message
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Sends an instant message to all participants in the conversation. This instant message will
 * be translated into each participant's text language.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param message The message to send.
 * @param outError error information.
 */
- (BOOL)sendTextMessageAsync:(nonnull void (^)(BOOL sent, NSError * _Nullable))completedHandler message:(nonnull NSString *)message error:(NSError * _Nullable * _Nullable)outError;

/**
 * Starts sending audio to the conversation service for speech recognition and translation. You
 * should subscribe to the Transcribing, and Transcribed events to receive conversation
 * translation results for yourself, and other participants in the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Starts sending audio to the conversation service for speech recognition and translation. You
 * should subscribe to the Transcribing, and Transcribed events to receive conversation
 * translation results for yourself, and other participants in the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)startTranscribingAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Stops sending audio to the conversation service. You will still receive Transcribing, and
 * Transcribed events for other participants in the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Stops sending audio to the conversation service. You will still receive Transcribing, and
 * Transcribed events for other participants in the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)stopTranscribingAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Subscribes to the Transcribed event which indicates that a final result has been transcribed.
 */
- (void)addTranscribedEventHandler:(nonnull SPXConversationTranslationEventHandler)eventHandler;

/**
 * Subscribes to the Transcribing event which indicates an that intermediate result has been transcribed.
 */
- (void)addTranscribingEventHandler:(nonnull SPXConversationTranslationEventHandler)eventHandler;

/**
 * Subscribes to the TextMessageReceived event that signals a translated text message from a conversation participant.
 */
- (void)addTextMessageReceivedEventHandler:(nonnull SPXConversationTranslationEventHandler)eventHandler;

/**
 * Subscribes to the Canceled event which indicates that an error occurred during transcription.
 */
- (void)addCanceledEventHandler:(nonnull SPXConversationTranslationCanceledEventHandler)eventHandler;

/**
 * Subscribes to the ConversationParticipantsChanged event that signals participants in the room have changed (e.g. a new participant joined).
 */
- (void)addConversationParticipantsChangedEventHandler:(nonnull SPXConversationParticipantsChangedEventHandler)eventHandler;

/**
 * Subscribes to the ConversationExpiration event that signals how many more minutes are left before the conversation expires.
 */
- (void)addConversationExpirationEventHandler:(nonnull SPXConversationExpirationEventHandler)eventHandler;

/**
 * Subscribes to the SessionStarted event using the specified handler.
 */
- (void)addSessionStartedEventHandler:(nonnull SPXConversationSessionEventHandler)eventHandler;

/**
 * Subscribes to the SessionStopped event using the specified handler.
 */
- (void)addSessionStoppedEventHandler:(nonnull SPXConversationSessionEventHandler)eventHandler;

@end
