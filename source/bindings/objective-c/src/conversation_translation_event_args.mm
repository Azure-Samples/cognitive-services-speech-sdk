//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXConversationTranslationEventArgs

- (instancetype)init:(const TranscriptionImpl::ConversationTranslationEventArgs&)e
{
    self = [super init:e];
    _result = [[SPXConversationTranslationResult alloc] init :e.GetResult()];

    return self;
}

@end

@implementation SPXConversationTranslationCanceledEventArgs

- (instancetype)init:(const TranscriptionImpl::ConversationTranslationCanceledEventArgs&)e
{
    self = [super init:e];
        
    return self;
}

@end

