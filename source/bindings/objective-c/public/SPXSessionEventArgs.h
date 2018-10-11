//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
  * Defines the payload for SessionStarted/SessionStopped events.
  */
SPX_EXPORT
@interface SPXSessionEventArgs : NSObject

/**
  * The session identifier.
  */
@property (copy, readonly, nonnull)NSString *sessionId;

@end
