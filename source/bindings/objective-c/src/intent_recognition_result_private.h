//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognition_result_private_h
#define intent_recognition_result_private_h

#import "intent_recognition_result.h"
#import "recognition_result_base_private.h"
#import "common_private.h"

@interface IntentRecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<IntentImpl::IntentRecognitionResult>)resultHandle;

@end

#endif /* intent_recognition_result_private_h */
