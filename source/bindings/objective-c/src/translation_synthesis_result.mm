//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "translation_synthesis_result_private.h"

#import "common_private.h"

@implementation TranslationSynthesisResult
{
    std::shared_ptr<TranslationImpl::TranslationSynthesisResult> resultImpl;
}

- (instancetype)init :(std::shared_ptr<TranslationImpl::TranslationSynthesisResult>)resultHandle
{
    self = [super init];
    resultImpl = resultHandle;

    switch (resultImpl->SynthesisStatus)
    {
        case TranslationImpl::SynthesisStatusCode::Success:
            _status = SynthesisStatus::SynthesisSuccess;
            break;
        case TranslationImpl::SynthesisStatusCode::SynthesisEnd:
            _status = SynthesisStatus::SynthesisEnd;
            break;
        case TranslationImpl::SynthesisStatusCode::Error:
            _status = SynthesisStatus::SynthesisError;
            break;
        default:
            // Todo error handling.
            NSLog(@"Unknown synthesis status");
            self = nil;
            break;
    }

    _failureReason = [NSString stringWithString:resultImpl->FailureReason];
    _audio = [NSData dataWithBytes:resultImpl->Audio.data() length:resultImpl->Audio.size()*sizeof(resultImpl->Audio[0])];

    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    _status = SynthesisStatus::SynthesisError;
    _failureReason = message;
    _audio = nil;
    return self;
}

- (void)dealloc
{
    if (resultImpl != nullptr) {
        resultImpl.reset();
    }
}

@end
