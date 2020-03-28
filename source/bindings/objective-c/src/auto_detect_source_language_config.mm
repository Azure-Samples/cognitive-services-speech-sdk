//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "auto_detect_source_language_config_private.h"
#import "source_language_config_private.h"
#import "util_private.h"
#import "exception.h"

@implementation SPXAutoDetectSourceLanguageConfiguration
{
    std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig> autoDetectSourceLanguageConfigImpl;
}

- (instancetype)init:(NSArray<NSString *> *)languages
{
    std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig> configImpl = nil;
    std::vector<std::string> srcLanguages;
    for (NSString* language in languages){        
        srcLanguages.push_back([language toSpxString]);
    }
    try {
        configImpl = SpeechImpl::AutoDetectSourceLanguageConfig::FromLanguages(srcLanguages);
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
        NSLog(@"Exception caught in creating SPXAutoDetectSourceLanguageConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];
}

- (nullable instancetype)init:(nonnull NSArray<NSString *> *)languages error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:languages];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithSourceLanguageConfigurations:(NSArray<SPXSourceLanguageConfiguration *> *)sourceLanguageConfigurations
{
    std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig> configImpl = nil;
    try {
        std::vector<std::shared_ptr<SpeechImpl::SourceLanguageConfig>> srcLanguageConfigurations;
        for (SPXSourceLanguageConfiguration* languageConfig in sourceLanguageConfigurations){
            srcLanguageConfigurations.push_back([languageConfig getHandle]);
        }
        configImpl = SpeechImpl::AutoDetectSourceLanguageConfig::FromSourceLanguageConfigs(srcLanguageConfigurations);
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
        NSLog(@"Exception caught in creating SPXAutoDetectSourceLanguageConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];
}


- (nullable instancetype)initWithSourceLanguageConfigurations:(nonnull NSArray<SPXSourceLanguageConfiguration *> *)sourceLanguageConfigurations 
                                                        error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self initWithSourceLanguageConfigurations:sourceLanguageConfigurations];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig>)configImpl
{
    self = [super init];
    autoDetectSourceLanguageConfigImpl = configImpl;
    return self;
}

- (std::shared_ptr<SpeechImpl::AutoDetectSourceLanguageConfig>)getHandle
{
    return autoDetectSourceLanguageConfigImpl;
}

@end
