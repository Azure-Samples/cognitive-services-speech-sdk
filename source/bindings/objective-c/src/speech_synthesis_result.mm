//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"
#import "exception.h"

@implementation SPXSpeechSynthesisResult
{
    std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultImpl;
}

- (instancetype)init:(std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)resultHandle
{
    self = [super init];
    if (resultHandle == nullptr) {
        _reason = SPXResultReason_Canceled;
        resultImpl = nullptr;
    }
    else
    {
        resultImpl = resultHandle;

        _reason = [Util fromResultReasonImpl:resultHandle->Reason];
        _resultId = [NSString StringWithStdString:resultHandle->ResultId];
        _audioData = [self getAudioData];
        _properties = [[SpeechSynthesisResultPropertyCollection alloc] initFrom:resultHandle];
    }
    
    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    _reason = SPXResultReason_Canceled;
    resultImpl = nullptr;
    return self;
}

- (NSData *)getAudioData
{
    if (resultImpl == nullptr)
        return nil;
    std::shared_ptr<std::vector< uint8_t >> data = resultImpl->GetAudioData();
    return [[NSData alloc] initWithBytes:data->data() length:resultImpl->GetAudioLength()];
}

- (std::shared_ptr<SpeechImpl::SpeechSynthesisResult>)getHandle
{
    return resultImpl;
}

- (void)dealloc {
    LogDebug(@"Speech synthesis result object deallocated.");
    if (!self->resultImpl)
    {
        NSLog(@"resultImpl is nil in speech synthesis result destructor");
        return;
    }
    try
    {
        self->resultImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in speech synthesizer destructor");
    }
}

@end

