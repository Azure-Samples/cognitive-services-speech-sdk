//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation Util

+ (SPXResultReason) fromResultReasonImpl:(SpeechImpl::ResultReason)reasonImpl
{
    SPXResultReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::ResultReason::NoMatch:
            reason = SPXResultReason_NoMatch;
            break;
        case SpeechImpl::ResultReason::Canceled:
            reason = SPXResultReason_Canceled;
            break;
        case SpeechImpl::ResultReason::RecognizingSpeech:
            reason = SPXResultReason_RecognizingSpeech;
            break;
        case SpeechImpl::ResultReason::RecognizedSpeech:
            reason = SPXResultReason_RecognizedSpeech;
            break;
        case SpeechImpl::ResultReason::RecognizingIntent:
            reason = SPXResultReason_RecognizingIntent;
            break;
        case SpeechImpl::ResultReason::RecognizedIntent:
            reason = SPXResultReason_RecognizedIntent;
            break;
        case SpeechImpl::ResultReason::TranslatingSpeech:
            reason = SPXResultReason_TranslatingSpeech;
            break;
        case SpeechImpl::ResultReason::TranslatedSpeech:
            reason = SPXResultReason_TranslatedSpeech;
            break;
        case SpeechImpl::ResultReason::SynthesizingAudio:
            reason = SPXResultReason_SynthesizingAudio;
            break;
        case SpeechImpl::ResultReason::SynthesizingAudioCompleted:
            reason = SPXResultReason_SynthesizingAudioCompleted;
            break;
        case SpeechImpl::ResultReason::SynthesizingAudioStarted:
            reason = SPXResultReason_SynthesizingAudioStarted;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown ResultReason value: %d.", (int)reasonImpl);
            reason = SPXResultReason_Canceled;
            break;
    }
    return reason;
}

+ (SPXCancellationReason) fromCancellationReasonImpl:(SpeechImpl::CancellationReason)reasonImpl
{
    SPXCancellationReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::CancellationReason::Error:
            reason = SPXCancellationReason_Error;
            break;
        case SpeechImpl::CancellationReason::EndOfStream:
            reason = SPXCancellationReason_EndOfStream;
            break;
        case SpeechImpl::CancellationReason::CancelledByUser:
            reason = SPXCancellationReason_CancelledByUser;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown CancellationReason value: %d.", (int)reasonImpl);
            reason = SPXCancellationReason_Error;
            break;
    }
    return reason;
}

+ (SPXCancellationErrorCode) fromCancellationErrorCodeImpl:(SpeechImpl::CancellationErrorCode)errorCodeImpl
{
    SPXCancellationErrorCode errorCode;
    switch (errorCodeImpl)
    {
        case SpeechImpl::CancellationErrorCode::NoError:
            errorCode = SPXCancellationErrorCode_NoError;
            break;
        case SpeechImpl::CancellationErrorCode::AuthenticationFailure:
            errorCode = SPXCancellationErrorCode_AuthenticationFailure;
            break;
        case SpeechImpl::CancellationErrorCode::BadRequest:
            errorCode = SPXCancellationErrorCode_BadRequest;
            break;
        case SpeechImpl::CancellationErrorCode::TooManyRequests:
            errorCode = SPXCancellationErrorCode_TooManyRequests;
            break;
        case SpeechImpl::CancellationErrorCode::Forbidden:
            errorCode = SPXCancellationErrorCode_Forbidden;
            break;
        case SpeechImpl::CancellationErrorCode::ConnectionFailure:
            errorCode = SPXCancellationErrorCode_ConnectionFailure;
            break;
        case SpeechImpl::CancellationErrorCode::ServiceTimeout:
            errorCode = SPXCancellationErrorCode_ServiceTimeout;
            break;
        case SpeechImpl::CancellationErrorCode::ServiceError:
            errorCode = SPXCancellationErrorCode_ServiceError;
            break;
        case SpeechImpl::CancellationErrorCode::ServiceUnavailable:
            errorCode = SPXCancellationErrorCode_ServiceUnavailable;
            break;
        case SpeechImpl::CancellationErrorCode::RuntimeError:
            errorCode = SPXCancellationErrorCode_RuntimeError;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown CancellationErrorCode value: %d.", (int)errorCodeImpl);
            errorCode = SPXCancellationErrorCode_RuntimeError;
            break;
    }
    return errorCode;
}

+ (SPXNoMatchReason) fromNoMatchReasonImpl:(SpeechImpl::NoMatchReason)reasonImpl
{
    SPXNoMatchReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::NoMatchReason::NotRecognized:
            reason = SPXNoMatchReason_NotRecognized;
            break;
        case SpeechImpl::NoMatchReason::InitialSilenceTimeout:
            reason = SPXNoMatchReason_InitialSilenceTimeout;
            break;
        case SpeechImpl::NoMatchReason::InitialBabbleTimeout:
            reason = SPXNoMatchReason_InitialBabbleTimeout;
            break;
        case SpeechImpl::NoMatchReason::KeywordNotRecognized:
            reason = SPXNoMatchReason_KeywordNotRecognized;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown NoMatchReason value: %d.", (int)reasonImpl);
            reason = SPXNoMatchReason_NotRecognized;
            break;
    }
    return reason;
}

+ (SPXParticipantChangedReason) fromParticipantChangedReasonImpl:(TranscriptionImpl::ParticipantChangedReason)reasonImpl
{
    SPXParticipantChangedReason reason;
    switch (reasonImpl)
    {
        case TranscriptionImpl::ParticipantChangedReason::JoinedConversation:
            reason = SPXParticipantChangedReason_JoinedConversation;
            break;
        case TranscriptionImpl::ParticipantChangedReason::LeftConversation:
            reason = SPXParticipantChangedReason_LeftConversation;
            break;
        case TranscriptionImpl::ParticipantChangedReason::Updated:
            reason = SPXParticipantChangedReason_Updated;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown ParticipantChangedReason value: %d.", (int)reasonImpl);
            break;
    }
    return reason;
}

+ (SPXOutputFormat) fromOutputFormatImpl:(SpeechImpl::OutputFormat)outputFormatImpl
{
    SPXOutputFormat outputFormat;
    switch (outputFormatImpl)
    {
        case SpeechImpl::OutputFormat::Simple:
            outputFormat = SPXOutputFormat_Simple;
            break;
        case SpeechImpl::OutputFormat::Detailed:
            outputFormat = SPXOutputFormat_Detailed;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown NoMatchReason value: %d.", (int)outputFormatImpl);
            outputFormat = SPXOutputFormat_Simple;
            break;
    }
    return outputFormat;
}

+ (int) getErrorNumberFromExceptionReason:(NSString * _Nonnull)message
{
    NSRange  searchedRange = NSMakeRange(0, [message length]);
    NSString * pattern = @"error code: (0x\\d+)";
    NSError * error = nil;
    NSRegularExpression * regex = [NSRegularExpression regularExpressionWithPattern:pattern options:0 error:&error];
    NSArray * matches = [regex matchesInString:message options:0 range:searchedRange];

    int errorNo = -1;
    for (NSTextCheckingResult * match in matches) {
        NSRange group1 = [match rangeAtIndex:1];
        NSScanner *scanner = [NSScanner scannerWithString:[message substringWithRange:group1]];
        unsigned int tmp;
        [scanner scanHexInt:&tmp];
        errorNo = tmp;
    }

    return errorNo;
}

+ (NSError * _Nonnull) getErrorFromException:(NSException * _Nonnull)exception
{
    NSLog(@"Util: getErrorFromException enter");
    NSError * error = nil;
    NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
    [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
    error = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                           code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    NSLog(@"Util: getErrorFromException exit");
    return error;
}

@end
