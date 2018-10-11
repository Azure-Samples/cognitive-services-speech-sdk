//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXRecognitionResult.h"

/**
  * Defines the translation recognition result.
  */
SPX_EXPORT
@interface SPXTranslationRecognitionResult : SPXRecognitionResult

/**
  * Translation results. Each item in the dictionary represents a translation result in one of target languages, where the key
  * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
  * @return the translated text results.
  */
@property (copy, readonly, nonnull)NSDictionary *translations;

@end
