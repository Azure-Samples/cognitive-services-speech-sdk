//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXSessionEventArgs.h"

/**
  * Defines payload for SpeechStartDetected and SpeechEndDetected events.
  */
@interface SPXRecognitionEventArgs : SPXSessionEventArgs

/**
  * Represents the message offset in ticks.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly)uint64_t offset;

@end
