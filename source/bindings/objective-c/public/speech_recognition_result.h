//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef SpeechRecognitionResult_h
#define SpeechRecognitionResult_h

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, RecognitionStatus)
{
    Recognized = 0,
    IntermediateResult,
    NoMatch,
    InitialSilenceTimeout,
    InitialBabbleTimeout,
    Canceled
};

@interface SpeechRecognitionResult : NSObject

@property (readonly) NSString* resultId;

@property (readonly) RecognitionStatus recognitionStatus;

@property (readonly) NSString* text;

@property (readonly) NSUInteger duration;

@property (readonly) NSUInteger offset;

@property (readonly) NSString* recognitionFailureReason;

// Todo: get property bag

@end

#endif /* SpeechRecognitionResult_h */
