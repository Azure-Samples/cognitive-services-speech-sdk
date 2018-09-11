//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_recognizer_private_h
#define speech_recognizer_private_h

#import "speech_recognizer.h"
#import "common_private.h"

@interface SpeechRecognizer (Private)

- (instancetype)init :(SpeechRecoSharedPtr)recoHandle;

- (void)onFinalResultEvent :(SpeechRecognitionEventArgs *)eventArgs;
- (void)onIntermediateResultEvent :(SpeechRecognitionEventArgs *)eventArgs;

@end
#endif /* speech_recognizer_private_h */
