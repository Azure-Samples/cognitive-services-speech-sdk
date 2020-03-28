//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechRecognitionResult.h"

/**
 * Represents auto detection source language result
 * Added in version 1.12.0
 */
SPX_EXPORT
@interface SPXAutoDetectSourceLanguageResult : NSObject

/**
 * The language result. It is empty if the service cannot detect the language.
 */
@property (copy, readonly, nullable)NSString *language;

/**
 * Initializes an SPXAutoDetectSourceLanguageResult object using SPXSpeechRecognitionResult.
 *
 * @param speechRecognitionResult the speech recognition result
 *
 * @return an instance of auto detection source language result.
 */

- (nonnull instancetype)init:(nonnull SPXSpeechRecognitionResult *)speechRecognitionResult;


@end