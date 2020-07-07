//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationTranslator.h"
#import "common_private.h"

@interface SPXConversationTranslator (Private)

- (instancetype)initWithImpl:(ConversationTranslatorSharedPtr)translatorHandle;

- (std::shared_ptr<TranscriptionImpl::ConversationTranslator>)getHandle;

@end
