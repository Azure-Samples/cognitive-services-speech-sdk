//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechRecognitionEventArgs.h"
#import "common_private.h"

@interface SPXSpeechRecognitionEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechRecognitionEventArgs&)e;

@end

@interface SPXSpeechRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechRecognitionCanceledEventArgs&)e;

@end
