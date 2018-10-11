//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "intent_recognition_result_private.h"
#import "common_private.h"

@implementation SPXIntentRecognitionResult
{
    std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<IntentImpl::IntentRecognitionResult>)resultHandle
{
    self = [super init :resultHandle];
    resultImpl = resultHandle;
    _intentId = [NSString StringWithStdString :resultHandle->IntentId];
    return self;
}

@end
