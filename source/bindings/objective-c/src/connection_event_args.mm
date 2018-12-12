//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "connection_event_args_private.h"
#import "session_event_args_private.h"

@implementation SPXConnectionEventArgs

- (instancetype)init :(const SpeechImpl::ConnectionEventArgs&)eventArgsHandle
{
    self = [super init:eventArgsHandle];
    return self;
}

@end

