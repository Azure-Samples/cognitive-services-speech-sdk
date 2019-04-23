//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXGrammar.h"
#import "SPXRecognizer.h"

/**
  * Represents a phrase list grammar for dynamic grammar scenarios.
  */
SPX_EXPORT
@interface SPXPhraseListGrammar : SPXGrammar

/**
 * Creates a phrase list grammar for the specified recognizer.
 * @param recognizer The recognizer from which to obtain the phrase list grammar.
 * @return a phrase list grammar instance.
 */
-(nullable instancetype)initWithRecognizer:(nonnull SPXRecognizer *)recognizer;

/**
  * Adds a simple phrase that may be spoken by the user.
  * @param phrase The phrase to be added.
  */
-(void)addPhrase:(nonnull NSString *)phrase;

/**
  * Clears all phrases from the phrase list grammar.
  */
-(void)clear;

@end
