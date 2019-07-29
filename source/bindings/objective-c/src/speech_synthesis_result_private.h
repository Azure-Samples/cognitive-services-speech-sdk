//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechSynthesisResult.h"
#import "common_private.h"

@interface SPXSpeechSynthesisResult (Private)

- (instancetype)init:(std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

- (std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)getHandle;

@end