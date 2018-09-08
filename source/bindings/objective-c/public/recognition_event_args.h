//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_event_args_h
#define recognition_event_args_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, RecognitionEventType)
{
    SpeechStartDetectedEvent = 0,
    SpeechEndDetectedEvent = 1
};

@interface RecognitionEventArgs : NSObject

@property (readonly) RecognitionEventType eventType;

@property (readonly) NSString* sessionId;

@property (readonly) NSUInteger offset;

@end

#endif /* recognition_event_args_h */
