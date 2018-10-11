//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSessionEventArgs.h"

/**
  * Defines the payload for SpeechStartDetected and SpeechEndDetected events.
  */
SPX_EXPORT
@interface SPXRecognitionEventArgs : SPXSessionEventArgs

/**
  * Represents the message offset in ticks.
  * A single tick represents one hundred nanoseconds or one ten-millionth of a second.
  */
@property (readonly)uint64_t offset;

@end
