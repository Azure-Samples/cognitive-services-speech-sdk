//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "SPXSpeechEnums.h"
#import "common_private.h"

@interface Util : NSObject

+ (SPXResultReason) fromResultReasonImpl:(SpeechImpl::ResultReason)reasonImpl;
+ (SPXCancellationReason) fromCancellationReasonImpl:(SpeechImpl::CancellationReason)reasonImpl;
+ (SPXCancellationErrorCode) fromCancellationErrorCodeImpl:(SpeechImpl::CancellationErrorCode)errorCodeImpl;
+ (SPXNoMatchReason) fromNoMatchReasonImpl:(SpeechImpl::NoMatchReason)reasonImpl;

@end
