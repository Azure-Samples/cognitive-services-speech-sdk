//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef session_event_args_h
#define session_event_args_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, SessionEventType)
{
    SessionStartedEvent = 0,
    SessionStoppedEvent = 1
};

@interface SessionEventArgs : NSObject

@property (readonly) SessionEventType eventType;

@property (readonly) NSString* sessionId;

@end

#endif /* session_event_args_h */
