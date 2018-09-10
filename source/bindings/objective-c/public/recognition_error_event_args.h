//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_error_event_args_h
#define recognition_error_event_args_h

#import <Foundation/Foundation.h>
#import "recognition_status.h"

@interface RecognitionErrorEventArgs : NSObject

@property (readonly) NSString* sessionId;

@property (readonly) RecognitionStatus status;

@property (readonly) NSString* failureReason;

@end

#endif /* recognition_error_event_args_h */
