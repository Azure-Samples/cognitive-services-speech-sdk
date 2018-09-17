//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognition_event_args_h
#define speech_recognition_event_args_h

#import <Foundation/Foundation.h>
#import "recognition_event_args.h"
#import "speech_recognition_result.h"

/**
  * Defines payload of speech recognizing/recognized event.
  */
@interface SpeechRecognitionEventArgs : RecognitionEventArgs

/**
  * The recognition result.
  */
@property (readonly) SpeechRecognitionResult *result;

@end


/**
  * Defines payload of speech recognition canceled events.
  */
@interface SpeechRecognitionCanceledEventArgs : SpeechRecognitionEventArgs

/**
  * The reason why the recognition was canceled.
  */
@property (readonly) CancellationReason reason;

/**
  * The error details of why the cancellation occurred.
  */
@property (readonly) NSString *errorDetails;

@end

#endif /* speech_recognition_event_args_h */
