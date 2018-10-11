//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXSpeechRecognitionResult.h"

/**
  * Defines the payload of speech recognizing/recognized event.
  */
SPX_EXPORT
@interface SPXSpeechRecognitionEventArgs : SPXRecognitionEventArgs

/**
  * The recognition result.
  */
@property (readonly, nonnull)SPXSpeechRecognitionResult *result;

@end


/**
  * Defines the payload of speech recognition canceled events.
  */
SPX_EXPORT
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
