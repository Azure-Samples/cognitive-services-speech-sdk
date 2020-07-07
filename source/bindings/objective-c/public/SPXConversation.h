//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXUser.h"
#import "SPXParticipant.h"
#import "SPXPropertyCollection.h"
#import "SPXSpeechConfiguration.h"

/**
 * Performs conversation management including add and remove participants.
 *
 * Added in version 1.13.0.
 */
SPX_EXPORT
@interface SPXConversation : NSObject

/**
 * The collection of properties and their values defined for this SPXConversation.
 */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * The unique identifier for the conversation.
 */
@property (nonatomic, copy, nullable)NSString *conversationId;

/**
 * The authorization token used to communicate with the service.
 * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires, 
 * the caller needs to refresh it by calling this setter with a new valid token. Otherwise, the recognizer will 
 * encounter errors during recognition.
 */
@property (nonatomic, copy, nullable)NSString *authorizationToken;

/**
 * Initializes a new instance of conversation using the specified speech configuration.
 *
 * @param speechConfiguration speech configuration.
 * @return a conversation instance.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of conversation using the specified speech configuration.
 *
 * @param speechConfiguration speech configuration.
 * @param outError error information.
 * @return a conversation instance.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes a new instance of conversation using the specified speech configuration and conversation id.
 *
 * @param speechConfiguration speech configuration.
 * @param conversationId a unqiue identification of your conversation.
 * @return a conversation instance.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration conversationId:(nonnull NSString *)conversationId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of conversation using the specified speech configuration and conversation id.
 *
 * @param speechConfiguration speech configuration.
 * @param conversationId a unqiue identification of your conversation. 
 * @param outError error information.
 * @return a conversation instance.
 */
- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration conversationId:(nonnull NSString *)conversationId error:(NSError * _Nullable * _Nullable)outError;

/**
 * Starts a conversation, and provides asynchronous callback with the information is conversation started successfully or not.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)startConversationAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Starts a conversation, and provides asynchronous callback with the information is conversation started successfully or not.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)startConversationAsync:(nonnull void (^)(BOOL started, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Ends a conversation, and provides asynchronous callback with the information is conversation ended successfully or not.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)endConversationAsync:(nonnull void (^)(BOOL ended, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Ends a conversation, and provides asynchronous callback with the information is conversation ended successfully or not.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)endConversationAsync:(nonnull void (^)(BOOL ended, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Deletes a conversation, and provides asynchronous callback with the information is conversation deletion completed.
 * After deleted, no one will be able to join the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)deleteConversationAsync:(nonnull void (^)(BOOL deleted, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Deletes a conversation, and provides asynchronous callback with the information is conversation deletion completed.
 * After deleted, no one will be able to join the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)deleteConversationAsync:(nonnull void (^)(BOOL deleted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Locks a conversation, and provides asynchronous callback with the information is conversation locking completed.
 * After locked, new participants are prevented from joining the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)lockConversationAsync:(nonnull void (^)(BOOL locked, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Locks a conversation, and provides asynchronous callback with the information is conversation locking completed.
 * After locked, new participants are prevented from joining the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)lockConversationAsync:(nonnull void (^)(BOOL locked, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Unlocks a conversation, and provides asynchronous callback with the information is conversation unlocking completed.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)unlockConversationAsync:(nonnull void (^)(BOOL unlocked, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Unlocks a conversation, and provides asynchronous callback with the information is conversation unlocking completed.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)unlockConversationAsync:(nonnull void (^)(BOOL unlocked, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Mute all other participants in the conversation. After this no other participants will have their speech recognitions broadcast, 
 * nor be able to send text messages.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)muteAllParticipantsAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Mute all other participants in the conversation. After this no other participants will have their speech recognitions broadcast, 
 * nor be able to send text messages.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)muteAllParticipantsAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Unmute all other participants in the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)unmuteAllParticipantsAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Unmute all other participants in the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)unmuteAllParticipantsAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Mute participant with a given userId in the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 */
- (void)muteParticipantAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Mute participant with a given userId in the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 * @param outError error information.
 */
- (BOOL)muteParticipantAsync:(nonnull void (^)(BOOL muted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

/**
 * Unmute participant with a given userId in the conversation.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 */
- (void)unmuteParticipantAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Unmute participant with a given userId in the conversation.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 * @param outError error information.
 */
- (BOOL)unmuteParticipantAsync:(nonnull void (^)(BOOL unmuted, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

/**
 * Adds a participant from a conversation using a user id.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 */
- (void)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Adds a participant from a conversation using a user id.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 * @param outError error information.
 */
- (BOOL)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

/**
 * Adds a participant from a conversation using a Participant object.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param participant A Participant instance.
 */
- (void)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Adds a participant from a conversation using a Participant object.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param participant A Participant instance.
 * @param outError error information.
 */
- (BOOL)addParticipantAsync:(nonnull void (^)(SPXParticipant * _Nullable, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant error:(NSError * _Nullable * _Nullable)outError;

/**
 * Adds a participant from a conversation using a User object.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param user A User instance.
 */
- (void)addParticipantAsync:(nonnull void (^)(SPXUser * _Nullable, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Adds a participant from a conversation using a User object.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param user A User instance.
 * @param outError error information.
 */
- (BOOL)addParticipantAsync:(nonnull void (^)(SPXUser * _Nullable, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user error:(NSError * _Nullable * _Nullable)outError;

/**
 * Remove a participant from a conversation using a user id.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 */
- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Remove a participant from a conversation using a user id.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param userId A user identifier.
 * @param outError error information.
 */
- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler userId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

/**
 * Remove a participant from a conversation using a Participant object.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param participant A Participant instance.
 */
- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Remove a participant from a conversation using a Participant object.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param participant A Participant instance.
 * @param outError error information.
 */
- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler participant:(nonnull SPXParticipant *)participant error:(NSError * _Nullable * _Nullable)outError;

/**
 * Remove a participant from a conversation using a User object.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param user A User instance.
 */
- (void)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Remove a participant from a conversation using a User object.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param user A User instance.
 * @param outError error information.
 */
- (BOOL)removeParticipantAsync:(nonnull void (^)(BOOL removed, NSError * _Nullable))completedHandler user:(nonnull SPXUser *)user error:(NSError * _Nullable * _Nullable)outError;

@end
