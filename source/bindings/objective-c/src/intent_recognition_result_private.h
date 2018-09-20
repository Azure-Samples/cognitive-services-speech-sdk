//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXIntentRecognitionResult.h"
#import "recognition_result_private.h"
#import "common_private.h"

@interface SPXIntentRecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<IntentImpl::IntentRecognitionResult>)resultHandle;

@end