//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXConversationEventArgs.h"
#import "common_private.h"

@interface SPXConversationExpirationEventArgs (Private)

- (instancetype)init :(const TranscriptionImpl::ConversationExpirationEventArgs&) eventArgsHandle;

@end

@interface SPXConversationParticipantsChangedEventArgs (Private)

- (instancetype)init :(const TranscriptionImpl::ConversationParticipantsChangedEventArgs&) eventArgsHandle;

@end
