//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationTranslationEventArgs.h"
#import "common_private.h"

@interface SPXConversationTranslationEventArgs (Private)

- (instancetype)init: (const TranscriptionImpl::ConversationTranslationEventArgs&)e;

@end

@interface SPXConversationTranslationCanceledEventArgs (Private)

- (instancetype)init: (const TranscriptionImpl::ConversationTranslationCanceledEventArgs&)e;

@end