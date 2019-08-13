//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"


@implementation SPXSpeechSynthesizer
{
    SpeechSynthSharedPtr speechSynthImpl;
}

- (instancetype)init:(SPXSpeechConfiguration *)speechConfiguration {
    try {
        self = [super init];
        speechSynthImpl = SpeechImpl::SpeechSynthesizer::FromConfig([speechConfiguration getHandle]);
        if (speechSynthImpl == nullptr)
            return nil;
        _properties = [[SpeechSynthesizerPropertyCollection alloc] initWithPropertyCollection:&speechSynthImpl->Properties from:speechSynthImpl];
        return self;
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
        NSLog(@"Exception caught when creating SPXSpeechSynthesizer in core.");
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
        self = [super init];
        speechSynthImpl = SpeechImpl::SpeechSynthesizer::FromConfig([speechConfiguration getHandle], [audioConfiguration getHandle]);
        if (speechSynthImpl == nullptr)
            return nil;
        _properties = [[SpeechSynthesizerPropertyCollection alloc] initWithPropertyCollection:&speechSynthImpl->Properties from:speechSynthImpl];
        return self;
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
        NSLog(@"Exception caught when creating SPXSpeechSynthesizer in core.");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithSpeechConfiguration:(SPXSpeechConfiguration *)speechConfiguration audioConfiguration:(SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError
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

- (void)setAuthorizationToken: (NSString *)token
{
    speechSynthImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:speechSynthImpl->GetAuthorizationToken()];
}

- (SPXSpeechSynthesisResult *)speakText:(nonnull NSString*)text NS_RETURNS_RETAINED
{
    if (speechSynthImpl == nullptr) {
        NSLog(@"SPXSynthesizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXSynthesizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultImpl = speechSynthImpl->SpeakTextAsync([text toSpxString]).get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXSpeechSynthesisResult alloc] init: resultImpl];
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

- (nullable SPXSpeechSynthesisResult *)speakText:(nonnull NSString*)text error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED
{
    try {
        return [self speakText:text];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (SPXSpeechSynthesisResult *)speakSsml:(nonnull NSString*)ssml NS_RETURNS_RETAINED
{
    if (speechSynthImpl == nullptr) {
        NSLog(@"SPXSynthesizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXSynthesizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultImpl = speechSynthImpl->SpeakSsmlAsync([ssml toSpxString]).get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXSpeechSynthesisResult alloc] init: resultImpl];
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

- (nullable SPXSpeechSynthesisResult *)speakSsml:(nonnull NSString*)ssml error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED
{
    try {
        return [self speakSsml:ssml];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (SPXSpeechSynthesisResult *)startSpeakingText:(nonnull NSString*)text NS_RETURNS_RETAINED
{
    if (speechSynthImpl == nullptr) {
        NSLog(@"SPXSynthesizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXSynthesizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultImpl = speechSynthImpl->StartSpeakingTextAsync([text toSpxString]).get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXSpeechSynthesisResult alloc] init: resultImpl];
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

- (nullable SPXSpeechSynthesisResult *)startSpeakingText:(nonnull NSString*)text error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED
{
    try {
        return [self startSpeakingText:text];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (SPXSpeechSynthesisResult *)startSpeakingSsml:(nonnull NSString*)ssml NS_RETURNS_RETAINED
{
    if (speechSynthImpl == nullptr) {
        NSLog(@"SPXSynthesizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXSynthesizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        std::shared_ptr<SpeechImpl::SpeechSynthesisResult> resultImpl = speechSynthImpl->StartSpeakingSsmlAsync([ssml toSpxString]).get();
        if (resultImpl == nullptr) {
            NSLog(@"No result available");
            NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                             reason:@"No result available"
                                                           userInfo:nil];
            [exception raise];
        }
        else
        {
            return [[SPXSpeechSynthesisResult alloc] init: resultImpl];
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

- (nullable SPXSpeechSynthesisResult *)startSpeakingSsml:(nonnull NSString*)ssml error:(NSError * _Nullable * _Nullable)outError NS_RETURNS_RETAINED
{
    try {
        return [self startSpeakingSsml:ssml];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}


- (void)addSynthesisStartedEventHandler:(SPXSpeechSynthesisEventHandler)eventHandler
{
    speechSynthImpl->SynthesisStarted.Connect(^(const SpeechImpl::SpeechSynthesisEventArgs & evt) {
        SPXSpeechSynthesisEventArgs *eventArgs = [[SPXSpeechSynthesisEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSynthesizingEventHandler:(SPXSpeechSynthesisEventHandler)eventHandler
{
    speechSynthImpl->Synthesizing.Connect(^(const SpeechImpl::SpeechSynthesisEventArgs & evt) {
        SPXSpeechSynthesisEventArgs *eventArgs = [[SPXSpeechSynthesisEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSynthesisCompletedEventHandler:(SPXSpeechSynthesisEventHandler)eventHandler
{
    speechSynthImpl->SynthesisCompleted.Connect(^(const SpeechImpl::SpeechSynthesisEventArgs & evt) {
        SPXSpeechSynthesisEventArgs *eventArgs = [[SPXSpeechSynthesisEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSynthesisCanceledEventHandler:(SPXSpeechSynthesisEventHandler)eventHandler
{
    speechSynthImpl->SynthesisCanceled.Connect(^(const SpeechImpl::SpeechSynthesisEventArgs & evt) {
        SPXSpeechSynthesisEventArgs *eventArgs = [[SPXSpeechSynthesisEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)addSynthesisWordBoundaryEventHandler:(SPXSpeechSynthesisWordBoundaryEventHandler)eventHandler
{
    speechSynthImpl->WordBoundary.Connect(^(const SpeechImpl::SpeechSynthesisWordBoundaryEventArgs & evt) {
        SPXSpeechSynthesisWordBoundaryEventArgs *eventArgs = [[SPXSpeechSynthesisWordBoundaryEventArgs alloc] init: evt];
        eventHandler(self, eventArgs);
    });
}

- (void)dealloc {
    LogDebug(@"Speech synthesizer object deallocated.");
    if (!self->speechSynthImpl)
    {
        NSLog(@"speechSynthImpl is nil in speech synthesizer destructor");
        return;
    }
    try
    {
        self->speechSynthImpl->SynthesisStarted.DisconnectAll();
        self->speechSynthImpl->Synthesizing.DisconnectAll();
        self->speechSynthImpl->SynthesisCompleted.DisconnectAll();
        self->speechSynthImpl->SynthesisCanceled.DisconnectAll();
        self->speechSynthImpl.reset();
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
