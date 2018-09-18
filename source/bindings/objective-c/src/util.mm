//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"

@implementation Util

+ (ResultReason) fromResultReasonImpl:(SpeechImpl::ResultReason)reasonImpl
{
    ResultReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::ResultReason::NoMatch:
            reason = ResultReason::NoMatch;
            break;
        case SpeechImpl::ResultReason::Canceled:
            reason = ResultReason::Canceled;
            break;
        case SpeechImpl::ResultReason::RecognizingSpeech:
            reason = ResultReason::RecognizingSpeech;
            break;
        case SpeechImpl::ResultReason::RecognizedSpeech:
            reason = ResultReason::RecognizedSpeech;
            break;
        case SpeechImpl::ResultReason::RecognizingIntent:
            reason = ResultReason::RecognizedIntent;
            break;
        case SpeechImpl::ResultReason::RecognizedIntent:
            reason = ResultReason::RecognizedIntent;
            break;
        case SpeechImpl::ResultReason::TranslatingSpeech:
            reason = ResultReason::TranslatingSpeech;
            break;
        case SpeechImpl::ResultReason::TranslatedSpeech:
            reason = ResultReason::TranslatedSpeech;
            break;
        case SpeechImpl::ResultReason::SynthesizingAudio:
            reason = ResultReason::SynthesizingAudio;
            break;
        case SpeechImpl::ResultReason::SynthesizingAudioCompleted:
            reason = ResultReason::SynthesizingAudioCompleted;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown ResultReason value: %d.", (int)reasonImpl);
            reason = ResultReason::Canceled;
            break;
    }
    return reason;
}

+ (CancellationReason) fromCancellationReasonImpl:(SpeechImpl::CancellationReason)reasonImpl
{
    CancellationReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::CancellationReason::Error:
            reason = CancellationReason::Error;
            break;
        case SpeechImpl::CancellationReason::EndOfStream:
            reason = CancellationReason::EndOfStream;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown CancellationReason value: %d.", (int)reasonImpl);
            reason = CancellationReason::Error;
            break;
    }
    return reason;
}

+ (NoMatchReason) fromNoMatchReasonImpl:(SpeechImpl::NoMatchReason)reasonImpl
{
    NoMatchReason reason;
    switch (reasonImpl)
    {
        case SpeechImpl::NoMatchReason::NotRecognized:
            reason = NoMatchReason::NotRecognized;
            break;
        case SpeechImpl::NoMatchReason::InitialSilenceTimeout:
            reason = NoMatchReason::InitialSilenceTimeout;
            break;
        case SpeechImpl::NoMatchReason::InitialBabbleTimeout:
            reason = NoMatchReason::InitialBabbleTimeout;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown NoMatchReason value: %d.", (int)reasonImpl);
            reason = NoMatchReason::NotRecognized;
            break;
    }
    return reason;
}

@end
