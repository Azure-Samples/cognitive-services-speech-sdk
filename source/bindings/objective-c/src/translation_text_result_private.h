//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXTranslationRecognitionResult.h"
#import "recognition_result_private.h"
#import "common_private.h"

@interface SPXTranslationRecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationRecognitionResult>)resultHandle;

@end
