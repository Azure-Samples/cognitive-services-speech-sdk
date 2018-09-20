//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

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
            reason = SPXResultReason_RecognizedIntent;
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
        default:
            // Todo error handling.
            NSLog(@"Unknown CancellationReason value: %d.", (int)reasonImpl);
            reason = SPXCancellationReason_Error;
            break;
    }
    return reason;
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
        default:
            // Todo error handling.
            NSLog(@"Unknown NoMatchReason value: %d.", (int)reasonImpl);
            reason = SPXNoMatchReason_NotRecognized;
            break;
    }
    return reason;
}

@end
