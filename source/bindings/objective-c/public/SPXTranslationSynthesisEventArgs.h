//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSessionEventArgs.h"
#import "SPXTranslationSynthesisResult.h"

/**
  * Defines the payload of the translation synthesis result event.
  */
SPX_EXPORT
@interface SPXTranslationSynthesisEventArgs : SPXSessionEventArgs

/**
  * The translation synthesis result.
  */
@property (readonly, nonnull)SPXTranslationSynthesisResult *result;

@end
