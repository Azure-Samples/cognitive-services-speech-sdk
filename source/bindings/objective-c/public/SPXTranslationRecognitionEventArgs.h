//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXRecognitionEventArgs.h"
#import "SPXTranslationRecognitionResult.h"


/**
  * Defines payload of text translation recognizing/recognized events.
  */
@interface SPXTranslationRecognitionEventArgs : SPXRecognitionEventArgs

/**
  * The translation text result.
  */
@property (readonly, nonnull)SPXTranslationRecognitionResult *result;

@end

/**
  * Defines payload of text translation canceled events.
  */
@interface SPXTranslationRecognitionCanceledEventArgs : SPXTranslationRecognitionEventArgs

/**
  * The reason why the text translation was canceled.
  */
@property (readonly)SPXCancellationReason reason;

/**
  * The error details of why the cancellation occurred.
  */
@property (copy, readonly, nullable)NSString *errorDetails;

@end
