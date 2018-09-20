//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXRecognitionEventArgs.h"
#import "common_private.h"

@interface SPXRecognitionEventArgs (Private)

- (instancetype)init :(const SpeechImpl::RecognitionEventArgs&) eventArgsHandle;

@end
