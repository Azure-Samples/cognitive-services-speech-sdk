//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXSpeechSynthesisEventArgs

- (instancetype)init:(const SpeechImpl::SpeechSynthesisEventArgs&)e
{
    self = [super init];
    _result = [[SPXSpeechSynthesisResult alloc] init:e.Result];
    return self;
}

@end

@implementation SPXSpeechSynthesisWordBoundaryEventArgs

- (instancetype)init: (const SpeechImpl::SpeechSynthesisWordBoundaryEventArgs&)e
{
    self = [super init];
    _audioOffset = (NSUInteger)e.AudioOffset;
    _textOffset = (NSUInteger)e.TextOffset;
    _wordLength = (NSUInteger)e.WordLength;
    return self;
}

@end
