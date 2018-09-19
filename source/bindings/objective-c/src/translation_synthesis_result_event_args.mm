//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation TranslationSynthesisEventArgs

- (instancetype)init:(const TranslationImpl::TranslationSynthesisEventArgs&)e
{
    self = [super init:e];

    _result = [[TranslationSynthesisResult alloc] init :e.GetResult()];

    return self;
}


@end

