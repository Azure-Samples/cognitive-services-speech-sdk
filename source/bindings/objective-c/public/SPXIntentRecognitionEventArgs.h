//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXRecognitionEventArgs.h"
#import "SPXIntentRecognitionResult.h"

/**
  * Defines payload for intent recognizing/recognized events.
  */
@interface SPXIntentRecognitionEventArgs : SPXRecognitionEventArgs

/**
  * Represents the intent recognition result.
  */
@property (readonly, nullable)SPXIntentRecognitionResult *result;

@end

/**
  * Define payload of intent recognition canceled result events.
  */
@interface SPXIntentRecognitionCanceledEventArgs : SPXIntentRecognitionEventArgs

/**
  * The reason the recognition was canceled.
  */
@property (readonly)SPXCancellationReason reason;

/**
  * The error details of why the occurred error.
  */
@property (copy, readonly, nullable)NSString *errorDetails;

@end
