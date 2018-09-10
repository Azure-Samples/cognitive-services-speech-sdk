//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "recognition_error_event_args_private.h"

@implementation RecognitionErrorEventArgs

- (instancetype)init :(NSString *)sessionId :(RecognitionStatus)status :(NSString *)failureReason
{
    self = [super init];
    _sessionId = sessionId;
    _status = status;
    _failureReason = failureReason;
    return self;
}

@end

