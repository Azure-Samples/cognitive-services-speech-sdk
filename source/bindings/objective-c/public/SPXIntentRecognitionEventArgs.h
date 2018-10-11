//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXIntentRecognitionResult.h"

/**
  * Defines the payload for intent recognizing/recognized events.
  */
SPX_EXPORT
@interface SPXIntentRecognitionEventArgs : SPXRecognitionEventArgs

/**
  * Represents the intent recognition result.
  */
@property (readonly, nullable)SPXIntentRecognitionResult *result;

@end

/**
  * Defines the payload of intent recognition canceled result events.
  */
SPX_EXPORT
@interface SPXIntentRecognitionCanceledEventArgs : SPXIntentRecognitionEventArgs

/**
  * The reason the recognition was canceled.
  */
@property (readonly)SPXCancellationReason reason;

/**
  * The error details of why the error occurred.
  */
@property (copy, readonly, nullable)NSString *errorDetails;

@end
