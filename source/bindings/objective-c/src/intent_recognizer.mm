//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXIntentRecognizer
{
    IntentRecoSharedPtr intentRecoImpl;
    dispatch_queue_t dispatchQueue;
    SPXAudioConfiguration *audioInputKeepAlive;
}

- (instancetype)init:(SPXSpeechConfiguration *)speechConfiguration
{
    try {
        auto recoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXIntentRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXSpeechConfiguration *)speechConfiguration error:(NSError * _Nullable * _Nullable)outError;
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

- (instancetype)initWithSpeechConfiguration:(SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = IntentImpl::IntentRecognizer::FromConfig([speechConfiguration getHandle], [audioConfiguration getHandle]);
        if (recoImpl == nullptr) {
            return nil;
        }
        if (audioConfiguration) {
            audioInputKeepAlive = audioConfiguration;
        }
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
        NSLog(@"Exception caught when creating SPXIntentRecognizer in core.");
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

- (instancetype)initWithImpl:(IntentRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->intentRecoImpl = recoHandle;
    if (!self || intentRecoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (void)dealloc {
    LogDebug(@"Intent recognizer object deallocated.");
    if (!self->intentRecoImpl) {
        NSLog(@"intentRecoImpl is nil in intent recognizer destructor");
        return;
    }
    try {
        self->intentRecoImpl->SessionStarted.DisconnectAll();
        self->intentRecoImpl->SessionStopped.DisconnectAll();
        self->intentRecoImpl->SpeechStartDetected.DisconnectAll();
        self->intentRecoImpl->SpeechEndDetected.DisconnectAll();
        self->intentRecoImpl->Recognizing.DisconnectAll();
        self->intentRecoImpl->Recognized.DisconnectAll();
        self->intentRecoImpl->Canceled.DisconnectAll();
        self->intentRecoImpl.reset();
        audioInputKeepAlive = nil;
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...) {
        NSLog(@"Exception caught in intent recognizer destructor");
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    intentRecoImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:intentRecoImpl->GetAuthorizationToken()];
}

- (void)addIntentFromPhrase:(NSString *)simplePhrase
{
    intentRecoImpl->AddIntent([simplePhrase toSpxString]);
}

- (void)addIntentFromPhrase:(NSString *)simplePhrase mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddIntent([simplePhrase toSpxString], [intentId toSpxString]);
}

- (void)addIntent: (NSString *)intentName fromModel:(SPXLanguageUnderstandingModel *)model
{
    intentRecoImpl->AddIntent([model getModelHandle], [intentName toSpxString]);
}

- (void)addIntent: (NSString *)intentName fromModel:(SPXLanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddIntent([model getModelHandle], [intentName toSpxString], [intentId toSpxString]);
}

- (void)addAllIntentsFromModel:(nonnull SPXLanguageUnderstandingModel *)model
{
    intentRecoImpl->AddAllIntents([model getModelHandle]);
}

- (void)addAllIntentsFromModel:(SPXLanguageUnderstandingModel *)model mappingToId:(NSString *)intentId
{
    intentRecoImpl->AddAllIntents([model getModelHandle], [intentId toSpxString]);
}

- (SPXIntentRecognitionResult *)recognizeOnce
{
    if (intentRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = intentRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXIntentRecognitionResult alloc] init: resultImpl];
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

- (nullable SPXIntentRecognitionResult *)recognizeOnce:(NSError * _Nullable * _Nullable)outError
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

- (void)recognizeOnceAsync:(void (^)(SPXIntentRecognitionResult *))resultReceivedHandler
{
    NSException * exception = nil;
    if (intentRecoImpl == nullptr) {
        auto result = [[SPXIntentRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        resultReceivedHandler(result);
        NSLog(@"SPXRecognizer handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXRecognizer handle is null"
                                          userInfo:nil];
        [exception raise];
    }
    
    __block std::future<std::shared_ptr<IntentImpl::IntentRecognitionResult>> futureObj;
    try {
        futureObj = intentRecoImpl->RecognizeOnceAsync();
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
        auto result = [[SPXIntentRecognitionResult alloc] initWithError: @"Runtime Exception"];
        resultReceivedHandler(result);
        [exception raise];
    }

    dispatch_async(dispatchQueue, ^{
        NSString * errorString;
        try {
            std::shared_ptr<IntentImpl::IntentRecognitionResult> resultImpl = futureObj.get();
            if (resultImpl == nullptr) {
                NSLog(@"No result is available");
                errorString = @"No result is available";
            }
            else {
                auto result = [[SPXIntentRecognitionResult alloc] init: resultImpl];
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
        
        auto result = [[SPXIntentRecognitionResult alloc] initWithError: errorString];
        resultReceivedHandler(result);
    });
}

- (BOOL)recognizeOnceAsync:(nonnull void (^)(SPXIntentRecognitionResult * _Nonnull))resultReceivedHandler error:(NSError * _Nullable * _Nullable)outError
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
    if (intentRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        intentRecoImpl->StartContinuousRecognitionAsync().get();
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
    if (intentRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        intentRecoImpl->StopContinuousRecognitionAsync().get();
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

- (void)addRecognizedEventHandler:(SPXIntentRecognitionEventHandler)eventHandler
{
    intentRecoImpl->Recognized.Connect(^(const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs & evt) {
        SPXIntentRecognitionEventArgs *eventArgs = [[SPXIntentRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addRecognizingEventHandler:(SPXIntentRecognitionEventHandler)eventHandler
{
    intentRecoImpl->Recognizing.Connect(^(const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionEventArgs & evt) {
        SPXIntentRecognitionEventArgs *eventArgs = [[SPXIntentRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXIntentRecognitionCanceledEventHandler)eventHandler
{
    intentRecoImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::Intent::IntentRecognitionCanceledEventArgs & evt) {
        SPXIntentRecognitionCanceledEventArgs *eventArgs = [[SPXIntentRecognitionCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
    intentRecoImpl->SessionStarted.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
    intentRecoImpl->SessionStopped.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    intentRecoImpl->SpeechStartDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    intentRecoImpl->SpeechEndDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

@end
