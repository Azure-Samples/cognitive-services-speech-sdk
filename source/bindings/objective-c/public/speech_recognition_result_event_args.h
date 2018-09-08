//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognitioneventargs_h
#define speechrecognitioneventargs_h

#import <Foundation/Foundation.h>
#import "speech_recognition_result.h"

@interface SpeechRecognitionResultEventArgs : NSObject

@property (readonly) NSString* sessionId;

@property (readonly) SpeechRecognitionResult* result;

@end

#endif /* speechrecognitioneventargs_h */
