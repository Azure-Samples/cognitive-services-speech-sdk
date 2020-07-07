//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXPropertyCollection.h"

/**
 * Represents a participant in a conversation transcribing session.
 *
 * Added in version 1.13.0.
 */
SPX_EXPORT
@interface SPXParticipant : NSObject

/**
 * The collection of properties and their values defined for this SPXParticipant.
 */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * The unique identifier for the participant.
 */
@property (nonatomic, copy, nullable)NSString *id;

/**
 * The user's avatar as an HTML hex string (e.g. FF0000 for red).
 */
@property (nonatomic, copy, nullable)NSString *avatar;

/**
 * The participant's display name. Please note that each participant within the same conversation must
 * have a different display name. Duplicate names within the same conversation are not allowed. You can
 * use the id property as another way to refer to each participant.
 */
@property (nonatomic, copy, nullable)NSString *displayName;

/**
 * The participant's preferred spoken language.
 */
@property (nonatomic, copy, nullable)NSString *preferredLanguage;

/**
 * The participant's voice signature.
 */
@property (nonatomic, copy, nullable)NSString *voiceSignature;

/**
 * Provides information is Participant muted.
 */
@property (nonatomic, readonly)BOOL isMuted;

/**
 * Provides information is participant using Text To Speech (TTS).
 */
@property (nonatomic, readonly)BOOL isUsingTts;

/**
 * Provides information is participant host.
 */
@property (nonatomic, readonly)BOOL isHost;

/**
 * Create a Participant using user id, her/his preferred language and her/his voice signature.
 * If voice signature is empty then user will not be identified.
 *
 * @param userId user's Id.
 * @param preferredLanguage users' preferred language.
 * @param voiceSignature user's voice signature.
 * @return The Participant object.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId preferredLanguage:(nonnull NSString *)preferredLanguage voiceSignature:(nonnull NSString *)voiceSignature
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Create a Participant using user id, her/his preferred language and her/his voice signature.
 * If voice signature is empty then user will not be identified.
 *
 * @param userId user's Id.
 * @param preferredLanguage users' preferred language.
 * @param voiceSignature user's voice signature.
 * @return The Participant object.
 * @param outError error information.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId preferredLanguage:(nonnull NSString *)preferredLanguage voiceSignature:(nonnull NSString *)voiceSignature error:(NSError * _Nullable * _Nullable)outError;

/**
 * Create a Participant using user id and her/his preferred language.
 *
 * @param userId user's Id.
 * @param preferredLanguage users' preferred language.
 * @return The Participant object.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId preferredLanguage:(nonnull NSString *)preferredLanguage
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Create a Participant using user id and her/his preferred language.
 *
 * @param userId user's Id.
 * @param preferredLanguage users' preferred language.
 * @return The Participant object.
 * @param outError error information.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId preferredLanguage:(nonnull NSString *)preferredLanguage error:(NSError * _Nullable * _Nullable)outError;

/**
 * Create a Participant using user id.
 *
 * @param userId user's Id.
 * @return The Participant object.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Create a Participant using user id.
 *
 * @param userId user's Id.
 * @return The Participant object.
 * @param outError error information.
 */
- (nullable instancetype)initFrom:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

@end
