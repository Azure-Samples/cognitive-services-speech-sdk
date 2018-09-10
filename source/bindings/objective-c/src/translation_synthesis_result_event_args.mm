//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "translation_synthesis_result_event_args_private.h"
#import "translation_synthesis_result_private.h"

#import "common_private.h"

@implementation TranslationSynthesisResultEventArgs

- (instancetype)init:(const TranslationImpl::TranslationSynthesisResultEventArgs&)e
{
    self = [super init];

    _sessionId = [NSString stringWithString:e.SessionId];
    _result = [[TranslationSynthesisResult alloc] init :e.GetResult()];

    return self;
}

@end

