//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "common_private.h"
#import "speechapi_private.h"
#import "phrase_list_grammar_private.h"

@implementation SPXPhraseListGrammar
{
    PhraseListGrammarSharedPtr grammarImpl;
}

- (instancetype)initWithRecognizer:(SPXRecognizer *)recognizer
{
    self->grammarImpl = SpeechImpl::PhraseListGrammar::FromRecognizer([recognizer getHandle]);
    if (self->grammarImpl == nullptr)
    {
        NSLog(@"Unable to create the phase list grammar.");
        return nil;
    }
    return self;
}

- (void)addPhrase:(nonnull const NSString *)phrase
{
    self->grammarImpl->AddPhrase([phrase string]);
}

- (void)clear
{
    self->grammarImpl->Clear();
}

@end
