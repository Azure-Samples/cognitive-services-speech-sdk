//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXKeywordRecognitionEventArgs.h"
#import "common_private.h"

@interface SPXKeywordRecognitionEventArgs (Private)

- (instancetype)init: (const KeywordImpl::KeywordRecognitionEventArgs&)e;

@end

@interface SPXKeywordRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const SpeechImpl::SpeechRecognitionCanceledEventArgs&)e;

@end
