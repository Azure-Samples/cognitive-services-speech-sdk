//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXConversationExpirationEventArgs

- (instancetype)init :(const TranscriptionImpl::ConversationExpirationEventArgs&)eventArgsHandle
{
    self = [super init:eventArgsHandle];
    _expirationTime = (NSUInteger)eventArgsHandle.ExpirationTime.count();
    return self;
}

@end

@implementation SPXConversationParticipantsChangedEventArgs

- (instancetype)init :(const TranscriptionImpl::ConversationParticipantsChangedEventArgs&)eventArgsHandle
{
    self = [super init:eventArgsHandle];
    _reason = [Util fromParticipantChangedReasonImpl:eventArgsHandle.Reason];

    auto participantsVector = eventArgsHandle.Participants;
    NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:eventArgsHandle.Participants.size()];
    for (std::vector<ParticipantSharedPtr>::iterator it = participantsVector.begin(); it != participantsVector.end(); ++it){
        [mutableArray addObject:[[SPXParticipant alloc] initWithImpl: *it]];        
    }
    _participants = mutableArray;

    return self;
}

@end
