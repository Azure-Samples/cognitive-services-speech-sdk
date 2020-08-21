//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation SPXKeywordRecognitionEventArgs

- (instancetype)init:(const KeywordImpl::KeywordRecognitionEventArgs&)e
{
    self = [super init:e];
    _result = [[SPXKeywordRecognitionResult alloc] init :e.GetResult()];

    return self;
}

@end

@implementation SPXKeywordRecognitionCanceledEventArgs

- (instancetype)init:(const SpeechImpl::SpeechRecognitionCanceledEventArgs&)e
{
    self = [super init:e];
    
    return self;
}

@end


