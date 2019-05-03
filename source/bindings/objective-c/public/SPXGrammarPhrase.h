//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"

/**
  * Represents a phrase that may be spoken by the user.
  * Added in version 1.5.0.
  */
SPX_EXPORT
@interface SPXGrammarPhrase : NSObject

-(nullable instancetype) initWithText:(nonnull NSString*)value;

@end

