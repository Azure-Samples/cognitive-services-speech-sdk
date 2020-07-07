//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversation.h"
#import "common_private.h"

@interface SPXConversation (Private)

- (_Nullable instancetype)initWithImpl:(ConversationSharedPtr)conversationHandle;

- (_Nonnull instancetype)initWithError:(NSString * _Nonnull)message;

- (std::shared_ptr<TranscriptionImpl::Conversation>)getHandle;

@end
