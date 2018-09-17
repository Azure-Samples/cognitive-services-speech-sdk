//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef translation_text_result_event_args_h
#define translation_text_result_event_args_h

#import <Foundation/Foundation.h>
#import "recognition_event_args.h"
#import "translation_text_result.h"


/**
  * Defines payload of text translation recognizing/recognized events.
  */
@interface TranslationTextResultEventArgs : RecognitionEventArgs

/**
  * The translation text result.
  */
@property (readonly) TranslationTextResult *result;

@end

/**
  * Defines payload of text translation canceled events.
  */
@interface TranslationTextResultCanceledEventArgs : TranslationTextResultEventArgs

/**
  * The reason why the text translation was canceled.
  */
@property (readonly) CancellationReason reason;

/**
  * The error details of why the cancellation occurred.
  */
@property (readonly) NSString *errorDetails;

@end

#endif /* translation_text_result_event_args_h */
