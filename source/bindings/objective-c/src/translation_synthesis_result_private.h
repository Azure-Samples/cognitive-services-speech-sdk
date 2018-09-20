//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXTranslationSynthesisResult.h"
#import "common_private.h"

@interface SPXTranslationSynthesisResult (Private)

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationSynthesisResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

@end