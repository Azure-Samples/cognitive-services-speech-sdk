//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef util_private_h
#define util_private_h

#import <Foundation/Foundation.h>
#import "speech_enums.h"
#import "common_private.h"

@interface Util : NSObject

+ (ResultReason) fromResultReasonImpl:(SpeechImpl::ResultReason)reasonImpl;
+ (CancellationReason) fromCancellationReasonImpl:(SpeechImpl::CancellationReason)reasonImpl;
+ (NoMatchReason) fromNoMatchReasonImpl:(SpeechImpl::NoMatchReason)reasonImpl;

@end

#endif /* util_private_h */
