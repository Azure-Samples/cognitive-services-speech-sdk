//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_error_event_args_private_h
#define recognition_error_event_args_private_h

#import "recognition_error_event_args.h"

#import "common_private.h"

@interface RecognitionErrorEventArgs (Private)

- (instancetype)init :(NSString *)sessionId :(RecognitionStatus)status :(NSString *)failureReason;

@end

#endif /* recognition_error_event_args_private_h */
