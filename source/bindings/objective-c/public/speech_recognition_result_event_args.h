//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognition_result_event_args_h
#define speech_recognition_result_event_args_h

#import <Foundation/Foundation.h>
#import "speech_recognition_result.h"

@interface SpeechRecognitionResultEventArgs : NSObject

@property (readonly) NSString* sessionId;

@property (readonly) SpeechRecognitionResult* result;

@end

#endif /* speech_recognition_result_event_args_h */
