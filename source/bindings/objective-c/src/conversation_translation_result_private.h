//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationTranslationResult.h"
#import "translation_text_result_private.h"
#import "common_private.h"

@interface SPXConversationTranslationResult (Private)

- (instancetype)init :(std::shared_ptr<TranscriptionImpl::ConversationTranslationResult>)resultHandle;

@end