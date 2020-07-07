//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationTranscriptionEventArgs.h"
#import "common_private.h"

@interface SPXConversationTranscriptionEventArgs (Private)

- (instancetype)init: (const TranscriptionImpl::ConversationTranscriptionEventArgs&)e;

@end

@interface SPXConversationTranscriptionCanceledEventArgs (Private)

- (instancetype)init: (const TranscriptionImpl::ConversationTranscriptionCanceledEventArgs&)e;

@end