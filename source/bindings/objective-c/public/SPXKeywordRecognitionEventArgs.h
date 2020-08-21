//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXSpeechRecognitionEventArgs.h"
#import "SPXKeywordRecognitionResult.h"

/**
 * Defines the payload for keyword recognizing/recognized events.
 */
SPX_EXPORT
@interface SPXKeywordRecognitionEventArgs : SPXRecognitionEventArgs

/**
 * Represents the keyword recognition result.
 */
@property (readonly, nullable)SPXKeywordRecognitionResult *result;

@end

/**
 * Defines the payload of keyword recognition canceled events.
 */
SPX_EXPORT
@interface SPXKeywordRecognitionCanceledEventArgs : SPXSpeechRecognitionCanceledEventArgs

@end
