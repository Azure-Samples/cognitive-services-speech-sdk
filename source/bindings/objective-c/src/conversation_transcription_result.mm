//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "conversation_transcription_result_private.h"
#import "common_private.h"

@implementation SPXConversationTranscriptionResult
{
    std::shared_ptr<TranscriptionImpl::ConversationTranscriptionResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<TranscriptionImpl::ConversationTranscriptionResult>)resultHandle
{
    self = [super init :resultHandle];
    resultImpl = resultHandle;
    _userId = [NSString StringWithStdString :resultHandle->UserId];
    _utteranceId = [NSString StringWithStdString :resultHandle->UtteranceId];
    return self;
}

@end
