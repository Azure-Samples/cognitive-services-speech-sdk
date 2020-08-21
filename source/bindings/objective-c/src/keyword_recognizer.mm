//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXKeywordRecognizer
{
    KeywordRecoSharedPtr keywordRecoImpl;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)init:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = KeywordImpl::KeywordRecognizer::FromConfig([audioConfiguration getHandle]);
        if (recoImpl == nullptr)
            return nil;
        return [self initWithImpl:recoImpl];
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
        [exception raise];
    }
    catch (...) {
        NSLog(@"Exception caught when creating SPXKeywordRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError;
{
    try {
        self = [self init:audioConfiguration];
        return self;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithImpl:(KeywordRecoSharedPtr)recoHandle
{
    self = [super init];
    self->keywordRecoImpl = recoHandle;
    if (!self || keywordRecoImpl == nullptr) {
        return nil;
    }
    else {
        _properties = [[KeywordRecognizerPropertyCollection alloc] initWithPropertyCollection:&keywordRecoImpl->Properties from:keywordRecoImpl];
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (void)dealloc {
    LogDebug(@"Keyword recognizer object deallocated.");
    if (!self->keywordRecoImpl)
    {
        NSLog(@"keywordRecoImpl is nil in keyword recognizer destructor");
        return;
    }
    try
    {
        self->keywordRecoImpl->Recognized.DisconnectAll();
        self->keywordRecoImpl->Canceled.DisconnectAll();
        self->keywordRecoImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in keyword recognizer destructor");
    }
}

- (void)recognizeOnceAsync:(void (^)(SPXKeywordRecognitionResult *))resultReceivedHandler keywordModel:(SPXKeywordRecognitionModel *)keywordModel
{
    NSException * exception = nil;
    if (keywordRecoImpl == nullptr) {
        auto result = [[SPXKeywordRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        resultReceivedHandler(result);
        NSLog(@"SPXRecognizer handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXRecognizer handle is null"
                                          userInfo:nil];
        [exception raise];
    }
    
    __block std::future<std::shared_ptr<KeywordImpl::KeywordRecognitionResult>> futureObj;
    try {
        futureObj = keywordRecoImpl->RecognizeOnceAsync([keywordModel getModelHandle]);
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        auto result = [[SPXKeywordRecognitionResult alloc] initWithError: @"Runtime Exception"];
        resultReceivedHandler(result);
        [exception raise];
    }

    dispatch_async(dispatchQueue, ^{
        NSString * errorString;
        try {
            std::shared_ptr<KeywordImpl::KeywordRecognitionResult> resultImpl = futureObj.get();
            if (resultImpl == nullptr) {
                NSLog(@"No result is available");
                errorString = @"No result is available";
            }
            else {
                auto result = [[SPXKeywordRecognitionResult alloc] init: resultImpl];
                resultReceivedHandler(result);
                return;
            }
        }
        catch (const std::exception &e) {
            NSLog(@"Exception caught in core: %s", e.what());
            errorString = [NSString StringWithStdString:e.what()];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"Exception with error code in core: %s", e.what());
            errorString = [NSString StringWithStdString:e.what()];
        }
        catch (...) {
            NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
            errorString = @"Runtime exception";
        }
        
        auto result = [[SPXKeywordRecognitionResult alloc] initWithError: errorString];
        resultReceivedHandler(result);
    });
}

- (BOOL)recognizeOnceAsync:(nonnull void (^)(SPXKeywordRecognitionResult * _Nonnull))resultReceivedHandler keywordModel:(SPXKeywordRecognitionModel *)keywordModel error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self recognizeOnceAsync:resultReceivedHandler keywordModel:keywordModel];
        return TRUE;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return FALSE;
}

- (void)stopRecognitionAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler
{
    __block NSException * exception = nil;
    if (keywordRecoImpl == nullptr) {
        NSLog(@"SPXKeywordRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXKeywordRecognizer handle is null"
                                                       userInfo:nil];
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }
    
    __block std::future<void> futureObj;
    try {
        futureObj = keywordRecoImpl->StopRecognitionAsync();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:[NSString StringWithStdString:e.what()]
                                          userInfo:nil];
    }
    catch (...) {
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"Runtime exception"
                                          userInfo:nil];
    }
    if (exception != nil) {
        completedHandler(NO, [Util getErrorFromException:exception]);
        [exception raise];
    }

    dispatch_async(dispatchQueue, ^{
        try {
            futureObj.get();
            completedHandler(YES, nil);
            return;
        }
        catch (const std::exception &e) {
            NSLog(@"dispatch_async: Exception caught in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (const SPXHR &hr) {
            auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
            NSLog(@"dispatch_async: Exception with error code in core: %s", e.what());
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:[NSString StringWithStdString:e.what()]
                                              userInfo:nil];
        }
        catch (...) {
            NSLog(@"%@: dispatch_async: Exception caught", NSStringFromSelector(_cmd));
            exception = [NSException exceptionWithName:@"SPXException"
                                                reason:@"Runtime exception"
                                              userInfo:nil];
        }
        NSError *error = [Util getErrorFromException:exception];
        completedHandler(NO, error);
    });
}

- (BOOL)stopRecognitionAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self stopRecognitionAsync:completedHandler];
        return TRUE;
    }
    catch (NSException *exception) {
        *outError = [Util getErrorFromException:exception];
    }
    return FALSE;
}

- (void)addRecognizedEventHandler:(SPXKeywordRecognitionEventHandler)eventHandler
{
    keywordRecoImpl->Recognized.Connect(^(const Microsoft::CognitiveServices::Speech::KeywordRecognitionEventArgs & evt) {
        SPXKeywordRecognitionEventArgs *eventArgs = [[SPXKeywordRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXKeywordRecognitionCanceledEventHandler)eventHandler
{
    keywordRecoImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs & evt) {
        SPXKeywordRecognitionCanceledEventArgs *eventArgs = [[SPXKeywordRecognitionCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

@end
