//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef intent_recognizer_private_h
#define intent_recognizer_private_h

#import "intent_recognizer.h"
#import "language_understanding_model_private.h"
#import "common_private.h"

@interface IntentRecognizer (Private)

- (instancetype)init :(IntentRecoSharedPtr)recoHandle;

- (void)onRecognizedEvent :(IntentRecognitionEventArgs *)eventArgs;
- (void)onRecognizingEvent :(IntentRecognitionEventArgs *)eventArgs;
- (void)onCanceledEvent :(IntentRecognitionCanceledEventArgs *)eventArgs;

@end
#endif /* intent_recognizer_private_h */
