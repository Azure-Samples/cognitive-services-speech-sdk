//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationTranscriber.h"
#import "common_private.h"

@interface SPXConversationTranscriber (Private)

- (_Nullable instancetype)initWithImpl:(ConversationTranscriberSharedPtr)transcriberHandle;

@end
