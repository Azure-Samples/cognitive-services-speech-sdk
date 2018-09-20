//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXSpeechRecognizer.h"
#import "common_private.h"

@interface SPXSpeechRecognizer (Private)

- (instancetype)initWithImpl:(SpeechRecoSharedPtr)recoHandle;

- (void)onRecognizedEvent :(SPXSpeechRecognitionEventArgs *)eventArgs;
- (void)onRecognizingEvent :(SPXSpeechRecognitionEventArgs *)eventArgs;
- (void)onCanceledEvent :(SPXSpeechRecognitionCanceledEventArgs *)eventArgs;

@end
