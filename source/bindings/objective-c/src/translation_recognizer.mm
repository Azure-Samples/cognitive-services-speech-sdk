//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXTranslationRecognizer
{
    TranslationRecoSharedPtr translationRecoImpl;
    dispatch_queue_t dispatchQueue;
}

- (instancetype)init:(SPXSpeechTranslationConfiguration *)translationConfiguration
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translationConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXTranslationRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)init:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self init:translationConfiguration];
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

- (instancetype)initWithSpeechTranslationConfiguration:(SPXSpeechTranslationConfiguration *)translationConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration
{
    try {
        auto recoImpl = TranslationImpl::TranslationRecognizer::FromConfig([translationConfiguration getHandle], [audioConfiguration getHandle]);
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
        NSLog(@"Exception caught when creating SPXTranslationRecognizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithSpeechTranslationConfiguration:(nonnull SPXSpeechTranslationConfiguration *)translationConfiguration audioConfiguration:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithSpeechTranslationConfiguration:translationConfiguration audioConfiguration:audioConfiguration];
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

- (instancetype)initWithImpl:(TranslationRecoSharedPtr)recoHandle
{
    self = [super initWith:recoHandle withParameters:&recoHandle->Properties];
    self->translationRecoImpl = recoHandle;
    if (!self || translationRecoImpl == nullptr) {
        return nil;
    }
    else
    {
        dispatchQueue = dispatch_queue_create("com.microsoft.cognitiveservices.speech", nil);
        return self;
    }
}

- (void)dealloc {
    LogDebug(@"translation recognizer object deallocated.");
    if (!self->translationRecoImpl)
    {
        NSLog(@"translationRecoImpl is nil in translation recognizer destructor");
        return;
    }

    try
    {
        self->translationRecoImpl->SessionStarted.DisconnectAll();
        self->translationRecoImpl->SessionStopped.DisconnectAll();
        self->translationRecoImpl->SpeechStartDetected.DisconnectAll();
        self->translationRecoImpl->SpeechEndDetected.DisconnectAll();
        self->translationRecoImpl->Recognizing.DisconnectAll();
        self->translationRecoImpl->Recognized.DisconnectAll();
        self->translationRecoImpl->Canceled.DisconnectAll();
        self->translationRecoImpl->Synthesizing.DisconnectAll();
        self->translationRecoImpl.reset();
    }
    catch (const std::exception &e) {
        NSLog(@"Exception caught in core: %s", e.what());
    }
    catch (const SPXHR &hr) {
        auto e = SpeechImpl::Impl::ExceptionWithCallStack(hr);
        NSLog(@"Exception with error code in core: %s", e.what());
    }
    catch (...)
    {
        NSLog(@"Exception caught in translation recognizer destructor");
    }
}

- (void)setAuthorizationToken: (NSString *)token
{
    translationRecoImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:translationRecoImpl->GetAuthorizationToken()];
}

- (SPXTranslationRecognitionResult *)recognizeOnce
{
    if (translationRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = translationRecoImpl->RecognizeOnceAsync().get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXTranslationRecognitionResult alloc] init: resultImpl];
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

- (nullable SPXTranslationRecognitionResult *)recognizeOnce:(NSError * _Nullable * _Nullable)outError
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

- (void)recognizeOnceAsync:(void (^)(SPXTranslationRecognitionResult *))resultReceivedHandler
{
    NSException * exception = nil;
    if (translationRecoImpl == nullptr) {
        auto result = [[SPXTranslationRecognitionResult alloc] initWithError: @"SPXRecognizer has been closed."];
        resultReceivedHandler(result);
        NSLog(@"SPXRecognizer handle is null");
        exception = [NSException exceptionWithName:@"SPXException"
                                            reason:@"SPXRecognizer handle is null"
                                          userInfo:nil];
        [exception raise];
    }
    
    __block std::future<std::shared_ptr<TranslationImpl::TranslationRecognitionResult>> futureObj;
    try {
        futureObj = translationRecoImpl->RecognizeOnceAsync();
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
        auto result = [[SPXTranslationRecognitionResult alloc] initWithError: @"Runtime Exception"];
        resultReceivedHandler(result);
        [exception raise];
    }

    dispatch_async(dispatchQueue, ^{
        NSString * errorString;
        try {
            std::shared_ptr<TranslationImpl::TranslationRecognitionResult> resultImpl = futureObj.get();
            if (resultImpl == nullptr) {
                NSLog(@"No result is available");
                errorString = @"No result is available";
            }
            else {
                auto result = [[SPXTranslationRecognitionResult alloc] init: resultImpl];
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
        
        auto result = [[SPXTranslationRecognitionResult alloc] initWithError: errorString];
        resultReceivedHandler(result);
    });
}

- (BOOL)recognizeOnceAsync:(nonnull void (^)(SPXTranslationRecognitionResult * _Nonnull))resultReceivedHandler error:(NSError * _Nullable * _Nullable)outError
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
    if (translationRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        translationRecoImpl->StartContinuousRecognitionAsync().get();
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
    if (translationRecoImpl == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        translationRecoImpl->StopContinuousRecognitionAsync().get();
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

- (void)addRecognizedEventHandler:(SPXTranslationRecognitionEventHandler)eventHandler
{
    translationRecoImpl->Recognized.Connect(^(const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs & evt) {
        SPXTranslationRecognitionEventArgs *eventArgs = [[SPXTranslationRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addRecognizingEventHandler:(SPXTranslationRecognitionEventHandler)eventHandler
{
    translationRecoImpl->Recognizing.Connect(^(const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionEventArgs & evt) {
        SPXTranslationRecognitionEventArgs *eventArgs = [[SPXTranslationRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addCanceledEventHandler:(SPXTranslationRecognitionCanceledEventHandler)eventHandler
{
    translationRecoImpl->Canceled.Connect(^(const Microsoft::CognitiveServices::Speech::Translation::TranslationRecognitionCanceledEventArgs & evt) {
        SPXTranslationRecognitionCanceledEventArgs *eventArgs = [[SPXTranslationRecognitionCanceledEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSynthesizingEventHandler:(SPXTranslationSynthesisEventHandler)eventHandler
{
    translationRecoImpl->Synthesizing.Connect(^(const Microsoft::CognitiveServices::Speech::Translation::TranslationSynthesisEventArgs & evt) {
        SPXTranslationSynthesisEventArgs *eventArgs = [[SPXTranslationSynthesisEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStartedEventHandler:(SPXSessionEventHandler)eventHandler
{
    translationRecoImpl->SessionStarted.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSessionStoppedEventHandler:(SPXSessionEventHandler)eventHandler
{
    translationRecoImpl->SessionStopped.Connect(^(const Microsoft::CognitiveServices::Speech::SessionEventArgs & evt) {
        SPXSessionEventArgs *eventArgs = [[SPXSessionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechStartDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    translationRecoImpl->SpeechStartDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSpeechEndDetectedEventHandler:(SPXRecognitionEventHandler)eventHandler
{
    translationRecoImpl->SpeechEndDetected.Connect(^(const Microsoft::CognitiveServices::Speech::RecognitionEventArgs & evt) {
        SPXRecognitionEventArgs *eventArgs = [[SPXRecognitionEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addTargetLanguage:(NSString *)lang
{
    translationRecoImpl->AddTargetLanguage([lang toSpxString]);
}

- (void)removeTargetLanguage:(NSString *)lang
{
    translationRecoImpl->RemoveTargetLanguage([lang toSpxString]);
}

- (NSArray *)targetLanguages
{
    auto langsVector = translationRecoImpl->GetTargetLanguages();
    NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:langsVector.size()];
    for (std::vector<std::string>::iterator it = langsVector.begin(); it != langsVector.end(); ++it){
        [mutableArray addObject:[NSString StringWithStdString:*it]];
    }
    return mutableArray;
}
@end
