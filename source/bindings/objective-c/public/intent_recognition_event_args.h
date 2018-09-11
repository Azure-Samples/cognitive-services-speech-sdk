//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognition_event_args_h
#define intent_recognition_event_args_h

#import <Foundation/Foundation.h>
#import "intent_recognition_result.h"

@interface IntentRecognitionEventArgs : NSObject

@property (readonly) NSString* sessionId;

@property (readonly) IntentRecognitionResult* result;

@end

#endif /* intent_recognition_event_args_h */
