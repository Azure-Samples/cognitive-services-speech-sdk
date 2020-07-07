//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "SPXSpeechEnums.h"
#import "common_private.h"

@interface Util : NSObject

+ (SPXResultReason) fromResultReasonImpl:(SpeechImpl::ResultReason)reasonImpl;
+ (SPXCancellationReason) fromCancellationReasonImpl:(SpeechImpl::CancellationReason)reasonImpl;
+ (SPXCancellationErrorCode) fromCancellationErrorCodeImpl:(SpeechImpl::CancellationErrorCode)errorCodeImpl;
+ (SPXNoMatchReason) fromNoMatchReasonImpl:(SpeechImpl::NoMatchReason)reasonImpl;
+ (SPXParticipantChangedReason) fromParticipantChangedReasonImpl:(TranscriptionImpl::ParticipantChangedReason)reasonImpl;
+ (SPXOutputFormat) fromOutputFormatImpl:(SpeechImpl::OutputFormat)outputFormatImpl;
+ (int) getErrorNumberFromExceptionReason:(NSString * _Nonnull)message;
+ (NSError * _Nonnull) getErrorFromException:(NSException * _Nonnull)exception;

@end
