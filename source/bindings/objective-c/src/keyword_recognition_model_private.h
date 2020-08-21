//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXKeywordRecognitionModel.h"
#import "common_private.h"

@interface SPXKeywordRecognitionModel (Private)

- (std::shared_ptr<KeywordImpl::KeywordRecognitionModel>)getModelHandle;

@end