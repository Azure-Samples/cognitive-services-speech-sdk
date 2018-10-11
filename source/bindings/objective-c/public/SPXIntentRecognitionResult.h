//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionResult.h"

/**
  * Defines the result of intent recognition.
  */
SPX_EXPORT
@interface SPXIntentRecognitionResult : SPXRecognitionResult


/**
  * A string that represents an intent identifier to be recognized.
  */
@property (copy, readonly, nullable)NSString *intentId;

@end
