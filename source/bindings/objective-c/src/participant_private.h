//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXParticipant.h"
#import "common_private.h"

@interface SPXParticipant (Private)

- (instancetype)initWithImpl:(ParticipantSharedPtr)participantHandle;

- (instancetype)initWithError:(NSString *)message;

- (ParticipantSharedPtr)getHandle;

@end
