//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation IntentRecognitionEventArgs

- (instancetype)init:(const IntentImpl::IntentRecognitionEventArgs&)e
{
    self = [super init:e];
    _result = [[IntentRecognitionResult alloc] init :e.GetResult()];

    return self;
}

@end

@implementation IntentRecognitionCanceledEventArgs

- (instancetype)init:(const IntentImpl::IntentRecognitionCanceledEventArgs&)e
{
    self = [super init:e];
    auto cancellationDetails = e.GetCancellationDetails();
    _reason = [Util fromCancellationReasonImpl:cancellationDetails->Reason];
    _errorDetails = [NSString stringWithString:cancellationDetails->ErrorDetails];
    
    return self;
}

@end

