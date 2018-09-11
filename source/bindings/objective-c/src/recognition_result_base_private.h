//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_result_base_private_h
#define recognition_result_base_private_h

#import "recognition_result_base.h"
#import "common_private.h"

@interface RecognitionResultBase (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

@end

#endif /* recognition_result_base_private_h */
