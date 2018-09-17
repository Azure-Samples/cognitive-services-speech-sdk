//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_result_private_h
#define recognition_result_private_h

#import "recognition_result.h"
#import "common_private.h"

@interface RecognitionResult (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle;

- (instancetype)initWithError:(NSString *)message;

- (std::shared_ptr<SpeechImpl::RecognitionResult>)getHandle;

@end

@interface CancellationDetails (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::CancellationDetails>)handle;

@end

@interface NoMatchDetails (Private)

- (instancetype)init :(std::shared_ptr<SpeechImpl::NoMatchDetails>)handle;

@end

#endif /* recognition_result_private_h */
