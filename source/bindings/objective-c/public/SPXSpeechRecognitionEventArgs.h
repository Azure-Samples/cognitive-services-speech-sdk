//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXRecognitionEventArgs.h"
#import "SPXSpeechRecognitionResult.h"

/**
  * Defines payload of speech recognizing/recognized event.
  */
@interface SPXSpeechRecognitionEventArgs : SPXRecognitionEventArgs

/**
  * The recognition result.
  */
@property (readonly, nonnull)SPXSpeechRecognitionResult *result;

@end


/**
  * Defines payload of speech recognition canceled events.
  */
@interface SPXSpeechRecognitionCanceledEventArgs : SPXSpeechRecognitionEventArgs

/**
  * The reason why the recognition was canceled.
  */
@property (readonly)SPXCancellationReason reason;

/**
  * The error details of why the cancellation occurred.
  */
@property (copy, readonly, nullable)NSString *errorDetails;

@end
