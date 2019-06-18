//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXSpeechRecognizer
{
    SpeechRecoSharedPtr speechRecoImpl;
    dispatch_queue_t dispatchQueue;

    RecognizerPropertyCollection *propertyCollection;
}

- (instancetype)init:(SPXSpeechConfiguration *)speechConfiguration {
     try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXSpeechRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self init:speechConfiguration];
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

- (instancetype)initWithSpeechConfiguration:(SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration {
    try {
        auto recoImpl = SpeechImpl::SpeechRecognizer::FromConfig([speechConfiguration getHandle], [audioConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXSpeechRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithSpeechConfiguration:(nonnull SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithSpeechConfiguration:speechConfiguration audioConfiguration:audioConfiguration];
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

- (instancetype)initWithImpl:(SpeechRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->speechRecoImpl = recoHandle;
    if (!self || speechRecoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (void)dealloc {
    NSLog(@"Speech recognizer object deallocated.");
    if (!self->speechRecoImpl)
    {
        NSLog(@"speechRecoImpl is nil in speech recognizer destructor");
        return;
    }

    try
    {
        self->speechRecoImpl->SessionStarted.DisconnectAll();
        self->speechRecoImpl->SessionStopped.DisconnectAll();
        self->speechRecoImpl->SpeechStartDetected.DisconnectAll();
        self->speechRecoImpl->SpeechEndDetected.DisconnectAll();
        self->speechRecoImpl->Recognizing.DisconnectAll();
        self->speechRecoImpl->Recognized.DisconnectAll();
        self->speechRecoImpl->Canceled.DisconnectAll();
        self->speechRecoImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in speech recognizer destructor");
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    speechRecoImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:speechRecoImpl->GetAuthorizationToken()];
}

- (NSString *)endpointId
{
    return [NSString StringWithStdString:speechRecoImpl->GetEndpointId()];
}

- (SPXSpeechRecognitionResult *)recognizeOnce
{
    if (speechRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = speechRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXSpeechRecognitionResult alloc] init: resultImpl];
        }
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
        NSLog(@"%@: Exception caught", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable SPXSpeechRecognitionResult *)recognizeOnce:(NSError * _Nullable * _Nullable)outError
{
    try {
        return [self recognizeOnce];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (void)recognizeOnceAsync:(nonnull void (^)(SPXSpeechRecognitionResult * _Nonnull))resultReceivedHandler;
{
    NSException * exception;
    if (speechRecoImpl == nullptr) {
        auto result = [[SPXSpeechRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        dispatch_async(dispatchQueue, ^{
            resultReceivedHandler(result);
        });
        NSLog(@"SPXRecognizer handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        auto future = speechRecoImpl->RecognizeOnceAsync();
        std::shared_ptr<SpeechImpl::SpeechRecognitionResult> resultImpl = future.get();
        if (resultImpl == nullptr) {
            NSLog(@"No result is available");
            exception = [NSException exceptionWithName:@"SPXException"
                                                            reason:@"No result available"
                                                          userInfo:nil];
        }
        else
        {
            auto result = [[SPXSpeechRecognitionResult alloc] init: resultImpl];
            dispatch_async(dispatchQueue, ^{
                resultReceivedHandler(result);
            });
            return;
        }
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
        exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:[NSString StringWithStdString:e.what()]
                                                       userInfo:nil];
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
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

    auto result = [[SPXSpeechRecognitionResult alloc] initWithError: @"Runtime Exception"];
    dispatch_async(dispatchQueue, ^{
        resultReceivedHandler(result);
    });
    [exception raise];
}

- (BOOL)recognizeOnceAsync:(nonnull void (^)(SPXSpeechRecognitionResult * _Nonnull))resultReceivedHandler error:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self recognizeOnceAsync:resultReceivedHandler];
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

- (void)startContinuousRecognition
{
    if (speechRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        speechRecoImpl->StartContinuousRecognitionAsync().get();
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
        NSLog(@"Exception caught in startContinuousRecognition");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
}

- (BOOL)startContinuousRecognition:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self startContinuousRecognition];
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

- (void)stopContinuousRecognition
{
    if (speechRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        speechRecoImpl->StopContinuousRecognitionAsync().get();
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
        NSLog(@"Exception caught in stopContinuousRecognition");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
}

- (BOOL)stopContinuousRecognition:(NSError * _Nullable * _Nullable)outError
{
    try {
        [self stopContinuousRecognition];
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

- (void)addRecognizedEventHandler:(SPXSpeechRecognitionEventHandler)eventHandler
{
    speechRecoImpl->Recognized.Connect(^(const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs & evt) {
        SPXSpeechRecognitionEventArgs *eventArgs = [[SPXSpeechRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addRecognizingEventHandler:(SPXSpeechRecognitionEventHandler)eventHandler
{
    speechRecoImpl->Recognizing.Connect(^(const Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs & evt) {
        SPXSpeechRecognitionEventArgs *eventArgs = [[SPXSpeechRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXSpeechRecognitionCanceledEventHandler)eventHandler
{
    speechRecoImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::SpeechRecognitionCanceledEventArgs & evt) {
        SPXSpeechRecognitionCanceledEventArgs *eventArgs = [[SPXSpeechRecognitionCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
    speechRecoImpl->SessionStarted.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
    speechRecoImpl->SessionStopped.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    speechRecoImpl->SpeechStartDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    speechRecoImpl->SpeechEndDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

@end
