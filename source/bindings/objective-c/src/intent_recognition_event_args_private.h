//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "SPXIntentRecognitionEventArgs.h"
#import "common_private.h"

@interface SPXIntentRecognitionEventArgs (Private)

- (instancetype)init: (const IntentImpl::IntentRecognitionEventArgs&)e;

@end

@interface SPXIntentRecognitionCanceledEventArgs (Private)

- (instancetype)init: (const IntentImpl::IntentRecognitionCanceledEventArgs&)e;

@end