//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
 * User representation in the conversation type of scenarios.
 *
 * Added in version 1.13.0.
 */
SPX_EXPORT
@interface SPXUser : NSObject

/**
 * User ID in the conversation session.
 */
@property (nonatomic, copy, nullable)NSString *userId;

/**
 * Gets the User instance from the specified user id.
 *
 * @param userId The user's id.
 * @return The User instance.
 */
- (nullable instancetype)initFromUserId:(nonnull NSString *)userId
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Gets the User instance from the specified user id.
 *
 * @param userId The user's id.
 * @return The User instance.
 * @param outError error information.
 */
- (nullable instancetype)initFromUserId:(nonnull NSString *)userId error:(NSError * _Nullable * _Nullable)outError;

@end
