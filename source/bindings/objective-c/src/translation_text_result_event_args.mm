//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "translation_text_result_event_args_private.h"
#import "translation_text_result_private.h"

#import "common_private.h"

@implementation TranslationTextResultEventArgs

- (instancetype)init:(const TranslationImpl::TranslationTextResultEventArgs&)e
{
    self = [super init];

    _sessionId = [NSString stringWithString:e.SessionId];
    auto resultImpl = e.GetResult();
    _result = [[TranslationTextResult alloc] init :resultImpl];

    return self;
}

@end

