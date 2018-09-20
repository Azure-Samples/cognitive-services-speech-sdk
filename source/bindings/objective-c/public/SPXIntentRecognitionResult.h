//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXRecognitionResult.h"

/**
  * Defines result of intent recognition.
  */
@interface SPXIntentRecognitionResult : SPXRecognitionResult


/**
  * A string that represents the intent identifier being recognized.
  */
@property (copy, readonly, nullable)NSString *intentId;

@end
