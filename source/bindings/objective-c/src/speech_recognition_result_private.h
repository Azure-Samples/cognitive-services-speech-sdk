//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognitionresult_private_h
#define speechrecognitionresult_private_h

#import "speech_recognition_result.h"

#import "common_private.h"

@interface SpeechRecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::SpeechRecognitionResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

@end

#endif /* speechrecognitionresult_private_h */
