//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXIntentRecognizer.h"
#import "language_understanding_model_private.h"
#import "common_private.h"

@interface SPXIntentRecognizer (Private)

- (instancetype)initWithImpl:(IntentRecoSharedPtr)recoHandle;

- (void)onRecognizedEvent :(SPXIntentRecognitionEventArgs *)eventArgs;
- (void)onRecognizingEvent :(SPXIntentRecognitionEventArgs *)eventArgs;
- (void)onCanceledEvent :(SPXIntentRecognitionCanceledEventArgs *)eventArgs;

@end
