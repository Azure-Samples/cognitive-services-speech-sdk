//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_event_args_h
#define recognition_event_args_h

#import <Foundation/Foundation.h>
#import "session_event_args.h"

/**
  * Defines payload for SpeechStartDetected and SpeechEndDetected events.
  */
@interface RecognitionEventArgs : SessionEventArgs

/**
  * Represents the message offset
  */
@property (readonly) NSUInteger offset;

@end

#endif /* recognition_event_args_h */
