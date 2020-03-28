//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "source_language_config_private.h"
#import "util_private.h"
#import "exception.h"

@implementation SPXSourceLanguageConfiguration
{
    std::shared_ptr<SpeechImpl::SourceLanguageConfig> sourceLanguageConfigImpl;
}

- (instancetype)init:(NSString *)language;
{
    std::shared_ptr<SpeechImpl::SourceLanguageConfig> configImpl = nil;
    
    try {
        configImpl = SpeechImpl::SourceLanguageConfig::FromLanguage([language toSpxString]);
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
        NSLog(@"Exception caught in creating SPXSourceLanguageConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];
}

- (nullable instancetype)init:(nonnull NSString *)language error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:language];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithLanguage:(NSString *)language endpointId:(NSString *)endpointId
{
    std::shared_ptr<SpeechImpl::SourceLanguageConfig> configImpl = nil;
    
    try {
        configImpl = SpeechImpl::SourceLanguageConfig::FromLanguage([language toSpxString], [endpointId toSpxString]);
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
        NSLog(@"Exception caught in creating SPXSourceLanguageConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];    
}

- (nullable instancetype)initWithLanguage:(nonnull NSString *)language endpointId:(nonnull NSString *)endpointId error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self initWithLanguage:language endpointId:endpointId];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::SourceLanguageConfig>)configImpl
{
    self = [super init];
    sourceLanguageConfigImpl = configImpl;
    return self;
}

- (std::shared_ptr<SpeechImpl::SourceLanguageConfig>)getHandle
{
    return sourceLanguageConfigImpl;
}

@end