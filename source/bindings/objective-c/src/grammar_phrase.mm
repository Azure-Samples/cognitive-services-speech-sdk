//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "grammar_phrase_private.h"
#import "common_private.h"
#import "speechapi_private.h"

@implementation SPXGrammarPhrase
{
    std::shared_ptr<SpeechImpl::GrammarPhrase> grammarPhraseImpl;
}

- (instancetype)initWithText:(NSString *)text
{
    self->grammarPhraseImpl = SpeechImpl::GrammarPhrase::From([text toSpxString]);
    if (self->grammarPhraseImpl == nullptr)
    {
        NSLog(@"Unable to create the grammar phrase.");
        return nil;
    }
    return self;
}

@end
