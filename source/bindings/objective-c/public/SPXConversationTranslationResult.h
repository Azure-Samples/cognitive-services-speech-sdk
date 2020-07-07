//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXTranslationRecognitionResult.h"

/**
  * Defines the result of conversation translation.
  */
SPX_EXPORT
@interface SPXConversationTranslationResult : SPXTranslationRecognitionResult

/**
  * A string that represents the original language this result was in.
  */
@property (copy, readonly, nullable)NSString *originalLang;

/**
  * A string that represents the unique identifier for the participant this result is for.
  */
@property (copy, readonly, nullable)NSString *participantId;

@end
