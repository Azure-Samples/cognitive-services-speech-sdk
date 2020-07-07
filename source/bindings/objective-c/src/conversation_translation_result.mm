//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "conversation_translation_result_private.h"
#import "common_private.h"

@implementation SPXConversationTranslationResult
{
    std::shared_ptr<TranscriptionImpl::ConversationTranslationResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<TranscriptionImpl::ConversationTranslationResult>)resultHandle
{
    self = [super init :resultHandle];
    resultImpl = resultHandle;
    _originalLang = [NSString StringWithStdString :resultHandle->OriginalLanguage];
    _participantId = [NSString StringWithStdString :resultHandle->ParticipantId];    
    return self;
}

@end
