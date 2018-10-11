//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
  * Defines the translation synthesis result, i.e. the voice output of the translated text in the target language.
  */
SPX_EXPORT
@interface SPXTranslationSynthesisResult : NSObject

/**
 * The reason the result was created.
 */
@property (readonly)SPXResultReason reason;

/**
  * The voice output of the translated text in the target language.
  */
@property (copy, readonly, nullable)NSData *audio;

@end
