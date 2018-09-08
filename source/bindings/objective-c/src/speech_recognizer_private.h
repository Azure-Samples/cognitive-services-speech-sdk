//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognizer_private_h
#define speechrecognizer_private_h

#import "speech_recognizer.h"
#import "common_private.h"

@interface SpeechRecognizer (Private)

- (instancetype)init :(SpeechRecoSharedPtr)recoHandle;

- (void)onFinalResultEvent :(SpeechRecognitionResultEventArgs *)eventArgs;
- (void)onIntermediateResultEvent :(SpeechRecognitionResultEventArgs *)eventArgs;

@end
#endif /* speechrecognizer_private_h */
