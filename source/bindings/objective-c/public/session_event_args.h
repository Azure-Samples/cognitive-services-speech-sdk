//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef session_event_args_h
#define session_event_args_h

#import <Foundation/Foundation.h>

/**
  * Defines payload for SessionStarted/Stopped events.
  */
@interface SessionEventArgs : NSObject

/**
  * The session identifier.
  */
@property (readonly) NSString *sessionId;

@end

#endif /* session_event_args_h */
