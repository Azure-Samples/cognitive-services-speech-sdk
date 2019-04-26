//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speechapi_private.h"
#import "exception.h"

@implementation SPXRecognitionResult
{
    std::shared_ptr<SpeechImpl::RecognitionResult> resultImpl;
}

- (instancetype)init:(std::shared_ptr<SpeechImpl::RecognitionResult>)resultHandle
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
        _text = [NSString StringWithStdString:resultHandle->Text];
        _duration = resultHandle->Duration();
        _offset = resultHandle->Offset();
        _properties = [[RecognitionResultPropertyCollection alloc] initFrom:resultHandle];
    }
    
    return self;
}

- (instancetype)initWithError: (NSString *)message
{
    _reason = SPXResultReason_Canceled;
    resultImpl = nullptr;
    return self;
}

- (std::shared_ptr<SpeechImpl::RecognitionResult>)getHandle
{
    return resultImpl;
}

@end

@implementation SPXCancellationDetails

-(instancetype)initFromCanceledRecognitionResult:(SPXRecognitionResult *)recognitionResult
{
    std::shared_ptr<SpeechImpl::RecognitionResult> handle = [recognitionResult getHandle];
    if (handle == nullptr)
    {
        self = [super init];
        _reason = SPXCancellationReason_Error;
        _errorCode = SPXCancellationErrorCode_RuntimeError;
        _errorDetails = @"The result handle is null.";
        return self;
    }
    else
    {
        try {
            auto cancellationDetailsImpl = SpeechImpl::CancellationDetails::FromResult(handle);
            return [self initWithImpl: cancellationDetailsImpl];
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:[NSString StringWithStdString:e.what()]
                                                           userInfo:nil];
            UNUSED(exception);
            // [exception raise];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:[NSString StringWithStdString:e.what()]
                                                           userInfo:nil];
            UNUSED(exception);
            // [exception raise];
        }
        catch (...)
        {
            NSLog(@"Exception caught when creating SPXCancellationDetails in core. Check to make sure that recognition result has the reason Canceled.\nNOTE: This will raise an exception in the future!");
        }
        return nil;
    }
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::CancellationDetails>)handle
{
    self = [super init];
    if (handle == nullptr) {
        return nil;
    }
    else
    {
        _reason = [Util fromCancellationReasonImpl:handle->Reason];
        _errorCode = [Util fromCancellationErrorCodeImpl:handle->ErrorCode];
        _errorDetails = [NSString StringWithStdString:handle->ErrorDetails];
    }
    return self;
}

@end

@implementation SPXNoMatchDetails

-(instancetype)initFromNoMatchRecognitionResult:(SPXRecognitionResult *)recognitionResult
{
    std::shared_ptr<SpeechImpl::RecognitionResult> handle = [recognitionResult getHandle];
    if (handle == nullptr)
    {
        return nil;
    }
    else
    {
        try {
            auto noMatchDetailsImpl = SpeechImpl::NoMatchDetails::FromResult([recognitionResult getHandle]);
            return [self initWithImpl: noMatchDetailsImpl];
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s\nNOTE: This will raise an exception in the future!", e.what());
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:[NSString StringWithStdString:e.what()]
                                                           userInfo:nil];
            UNUSED(exception);
            // [exception raise];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s\nNOTE: This will raise an exception in the future!", e.what());
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:[NSString StringWithStdString:e.what()]
                                                           userInfo:nil];
            UNUSED(exception);
            // [exception raise];
        }
        catch (...)
        {
            NSLog(@"Exception caught when creating SPXNoMatchDetails in core. Check to make sure that recognition result has the reason NoMatch.\nNOTE: This will raise an exception in the future!");
        }
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::NoMatchDetails>)handle
{
    self = [super init];
    if (handle == nullptr)
    {
        return nil;
    }
    else
    {
        _reason = [Util fromNoMatchReasonImpl:handle->Reason];
    }
    return self;
}

@end
