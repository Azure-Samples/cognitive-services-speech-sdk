//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "exception.h"

@implementation SPXSpeechConfiguration
{
    std::shared_ptr<SpeechImpl::SpeechConfig> speechConfigImpl;
}

- (instancetype)initWithSubscription:(NSString *)subscriptionKey region:(NSString *)region
{
    try {
        auto configImpl = SpeechImpl::SpeechConfig::FromSubscription([subscriptionKey toSpxString], [region toSpxString]);
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
        NSLog(@"%@: Exception caught.", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (instancetype)initWithSubscription:(NSString *)subscriptionKey region:(NSString *)region error:(NSError * _Nullable * _Nullable)outError
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
        auto configImpl = SpeechImpl::SpeechConfig::FromAuthorizationToken([authToken toSpxString], [region toSpxString]);
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

- (nullable instancetype)initWithAuthorizationToken:(nonnull NSString *)authToken region:(nonnull NSString *)region error:(NSError * _Nullable * _Nullable)outError;
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
        auto configImpl = SpeechImpl::SpeechConfig::FromEndpoint([endpointUri toSpxString], [subscriptionKey toSpxString]);
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
        NSLog(@"%@: Exception caught.", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri subscription:(nonnull NSString *)subscriptionKey error:(NSError * _Nullable * _Nullable)outError;
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
        auto configImpl = SpeechImpl::SpeechConfig::FromEndpoint([endpointUri toSpxString]);
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
        NSLog(@"%@: Exception caught.", NSStringFromSelector(_cmd));
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime Exception"
                                                       userInfo:nil];
        [exception raise];
    }
    return nil;
}

- (nullable instancetype)initWithEndpoint:(nonnull NSString *)endpointUri error:(NSError * _Nullable * _Nullable)outError;
{
    try {
        self = [self initWithEndpoint:endpointUri];
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

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SpeechConfig>)configImpl
{
    self = [super init];
    if (!self || configImpl == nullptr) {
        return nil;
    }
    speechConfigImpl = configImpl;

    speechConfigImpl->SetProperty("SPEECHSDK-SPEECH-CONFIG-SYSTEM-LANGUAGE", "Objective-C");

    return self;
}

- (std::shared_ptr<SpeechImpl::SpeechConfig>)getHandle
{
    return speechConfigImpl;
}

- (void)setSpeechRecognitionLanguage: (NSString *)lang
{
    speechConfigImpl->SetSpeechRecognitionLanguage([lang toSpxString]);
}

- (NSString *)speechRecognitionLanguage
{
    return [NSString StringWithStdString:speechConfigImpl->GetSpeechRecognitionLanguage()];
}

- (void)setEndpointId: (NSString *)endpointId
{
    speechConfigImpl->SetEndpointId([endpointId toSpxString]);
}

- (NSString *)endpointId
{
    return [NSString StringWithStdString:speechConfigImpl->GetEndpointId()];
}

- (void)setAuthorizationToken: (NSString *)token
{
    speechConfigImpl->SetAuthorizationToken([token toSpxString]);
}

- (NSString *)authorizationToken
{
    return [NSString StringWithStdString:speechConfigImpl->GetAuthorizationToken()];
}

- (NSString *)subscriptionKey
{
    return [NSString StringWithStdString:speechConfigImpl->GetSubscriptionKey()];
}

- (NSString *)region
{
    return [NSString StringWithStdString:speechConfigImpl->GetRegion()];
}

- (void)setOutputFormat: (SPXOutputFormat)outputFormat
{
    speechConfigImpl->SetOutputFormat((SpeechImpl::OutputFormat)(int)outputFormat);
}

- (SPXOutputFormat)outputFormat
{
    return [Util fromOutputFormatImpl:speechConfigImpl->GetOutputFormat()];
}

-(void)setProxyUsingHost:(NSString *)proxyHostName Port:(unsigned int)proxyPort UserName:(NSString *)proxyUserName Password:(NSString *)proxyPassword
{
    if ((proxyHostName == NULL) || ([proxyHostName length] == 0))
    {
        [NSException raise:@"Invalid proxy host name" format:@"proxy host name is null or empty."];
    }
    if (proxyPort == 0)
    {
        [NSException raise:@"Invalid proxy port" format:@"proxy port cannot be 0."];
    }
    if ((proxyUserName == NULL) || (proxyPassword == NULL))
    {
        [NSException raise:@"Invalid proxy user name or password" format:@"Proxy user name or password is null."];
    }
    speechConfigImpl->SetProxy([proxyHostName toSpxString], proxyPort, [proxyUserName toSpxString], [proxyPassword toSpxString]);
}

-(BOOL)setProxyUsingHost:(nonnull NSString *)proxyHostName Port:(uint32_t)proxyPort UserName:(nullable NSString *)proxyUserName Password:(nullable NSString *)proxyPassword error:(NSError * _Nullable * _Nullable)outError;
{
    try {
        [self setProxyUsingHost:proxyHostName Port:proxyPort UserName:proxyUserName Password:proxyPassword];
        return true;
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:0x5 userInfo:errorDict];
        return false;
    }
}

-(NSString *)getPropertyByName:(NSString *)name
{
    return [NSString StringWithStdString:speechConfigImpl->GetProperty([name toSpxString])];
}

-(void)setPropertyTo:(NSString *)value byName:(NSString *)name
{
    speechConfigImpl->SetProperty([name toSpxString], [value toSpxString]);
}

-(NSString *)getPropertyById:(SPXPropertyId)propertyId
{
    return [NSString StringWithStdString:speechConfigImpl->GetProperty((SpeechImpl::PropertyId)(int)propertyId)];
}

-(void)setPropertyTo:(NSString *)value byId:(SPXPropertyId)propertyId
{
    speechConfigImpl->SetProperty((SpeechImpl::PropertyId)(int)propertyId, [value toSpxString]);
}

-(void) setServicePropertyTo:(nonnull NSString *)value byName:(nonnull NSString*)name usingChannel:(SPXServicePropertyChannel)channel
{
    speechConfigImpl->SetServiceProperty([name toSpxString], [value toSpxString], (SpeechImpl::ServicePropertyChannel)(int)channel);
}

-(void)setProfanityOptionTo:(SPXSpeechConfigProfanityOption)profanity
{
    speechConfigImpl->SetProfanity((SpeechImpl::ProfanityOption)(int)profanity);
}

-(void)enableAudioLogging
{
    speechConfigImpl->EnableAudioLogging();
}

-(void)requestWordLevelTimestamps
{
    speechConfigImpl->RequestWordLevelTimestamps();
}

-(void)enableDictation
{
    speechConfigImpl->EnableDictation();
}

@end
