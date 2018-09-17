//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognition_event_args_h
#define intent_recognition_event_args_h

#import <Foundation/Foundation.h>
#import "recognition_event_args.h"
#import "intent_recognition_result.h"

/**
  * Defines payload for intent recognizing/recognized events.
  */
@interface IntentRecognitionEventArgs : RecognitionEventArgs

/**
  * Represents the intent recognition result.
  */
@property (readonly) IntentRecognitionResult *result;

@end

/**
  * Define payload of intent recognition canceled result events.
  */
@interface IntentRecognitionCanceledEventArgs : IntentRecognitionEventArgs

/**
  * The reason the recognition was canceled.
  */
@property (readonly) CancellationReason reason;

/**
  * The error details of why the occurred error.
  */
@property (readonly) NSString *errorDetails;

@end

#endif /* intent_recognition_event_args_h */
