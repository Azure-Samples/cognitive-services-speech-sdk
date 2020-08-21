//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXKeywordRecognizer.h"
#import "keyword_recognition_model_private.h"
#import "common_private.h"

@interface SPXKeywordRecognizer (Private)

- (instancetype)initWithImpl:(KeywordRecoSharedPtr)recoHandle;

@end
