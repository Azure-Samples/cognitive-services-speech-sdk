//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "keyword_recognition_result_private.h"
#import "common_private.h"

@implementation SPXKeywordRecognitionResult
{
    std::shared_ptr<KeywordImpl::KeywordRecognitionResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<KeywordImpl::KeywordRecognitionResult>)resultHandle
{
    self = [super init :resultHandle];
    resultImpl = resultHandle;
    return self;
}

- (std::shared_ptr<KeywordImpl::KeywordRecognitionResult>)getHandle
{
    return resultImpl;
}

@end
