//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSessionEventArgs.h"

/**
 * Defines the payload for conversation expiration event.
 *
 */
SPX_EXPORT
@interface SPXConversationExpirationEventArgs : SPXSessionEventArgs

/**
 * Represents the time left until the conversation expires.
 */
@property (readonly)NSUInteger expirationTime;

@end

/**
 * Defines the payload for conversation participant changed event.
 *
 */
SPX_EXPORT
@interface SPXConversationParticipantsChangedEventArgs : SPXSessionEventArgs

/**
 * The reason why the participant changed event was raised (e.g. a participant joined).
 */
@property (readonly)SPXParticipantChangedReason reason;

/**
 * Represents the the list of participant(s) that joined, left, or were updated.
 */
@property (nonatomic, copy, readonly, nonnull)NSArray *participants;

@end