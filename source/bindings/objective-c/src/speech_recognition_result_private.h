//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognition_result_private_h
#define speech_recognition_result_private_h

#import "speech_recognition_result.h"
#import "recognition_result_base_private.h"
#import "common_private.h"

@interface SpeechRecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::SpeechRecognitionResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

@end

#endif /* speech_recognition_result_private_h */
