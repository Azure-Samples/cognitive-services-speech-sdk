//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXSpeechTranslationConfiguration
{
    std::shared_ptr<TranslationImpl::SpeechTranslationConfig> speechTranslationConfigurationImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscriptionKey region:(NSString *)region
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromSubscription([subscriptionKey toSpxString], [region toSpxString]);
        if (configImpl == nullptr) {
            return nil;
        }
        return [self initWithImpl:configImpl];
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

- (nullable instancetype)initWithSubscription:(nonnull NSString *)subscriptionKey region:(nonnull NSString *)region error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithSubscription:subscriptionKey region:region];
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

- (instancetype)initWithAuthorizationToken:(NSString *)authToken region:(NSString *)region
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromAuthorizationToken([authToken toSpxString], [region toSpxString]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
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

- (nullable instancetype)initWithAuthorizationToken:(nonnull NSString *)authToken region:(nonnull NSString *)region error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithAuthorizationToken:authToken region:region];
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

- (instancetype)initWithEndpoint:(NSString *)endpointUri subscription:(NSString *)subscriptionKey
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromEndpoint([endpointUri toSpxString], [subscriptionKey toSpxString]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
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

- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithEndpoint:endpointUri subscription:subscriptionKey];
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


- (instancetype)initWithEndpoint:(NSString *)endpointUri
{
    try {
        auto configImpl = TranslationImpl::SpeechTranslationConfig::FromEndpoint([endpointUri toSpxString]);
        if (configImpl == nullptr)
            return nil;
        return [self initWithImpl:configImpl];
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

- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri error:(NSError * _Nullable * _Nullable)outError
{
    try {
        self = [self initWithEndpoint:endpointUri];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)translationConfigurationImpl
{
    self = [super initWithImpl:translationConfigurationImpl];
    if (!self || translationConfigurationImpl == nullptr) {
        return nil;
    }
    self->speechTranslationConfigurationImpl = translationConfigurationImpl;
    return self;
}

- (std::shared_ptr<TranslationImpl::SpeechTranslationConfig>)getHandle
{
    return speechTranslationConfigurationImpl;
}

- (void)addTargetLanguage:(NSString *)lang
{
    speechTranslationConfigurationImpl->AddTargetLanguage([lang toSpxString]);
}

- (void)removeTargetLanguage:(NSString *)lang
{
    speechTranslationConfigurationImpl->RemoveTargetLanguage([lang toSpxString]);
}

- (NSArray *)targetLanguages
{
    auto langsVector = speechTranslationConfigurationImpl->GetTargetLanguages();
    NSMutableArray *mutableArray = [[NSMutableArray alloc] initWithCapacity:langsVector.size()];
    for (std::vector<std::string>::iterator it = langsVector.begin(); it != langsVector.end(); ++it){
        [mutableArray addObject:[NSString StringWithStdString:*it]];
    }
    return mutableArray;
}

- (void)setVoiceName:(NSString *)voice
{
    speechTranslationConfigurationImpl->SetVoiceName([voice toSpxString]);
}

- (NSString *)voiceName
{
    return [NSString StringWithStdString:speechTranslationConfigurationImpl->GetVoiceName()];
}

@end

