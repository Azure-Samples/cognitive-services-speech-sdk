//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "speechapi_private.h"
#import "util_private.h"
#import "exception.h"

@implementation SPXPronunciationAssessmentConfiguration
{
    std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig> pronAssessmentConfigImpl;
}

- (instancetype)init:(nonnull NSString *)referenceText
       gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
         granularity:(SPXPronunciationAssessmentGranularity)granularity
        enableMiscue:(BOOL)enableMiscue
          scenarioId:(nonnull NSString *)scenarioId {
    std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig> configImpl = nil;

    try {
        configImpl = SpeechImpl::PronunciationAssessmentConfig::Create([referenceText toSpxString],
                                                                       (SpeechImpl::PronunciationAssessmentGradingSystem)(int)gradingSystem,
                                                                       (SpeechImpl::PronunciationAssessmentGranularity)(int)granularity,
                                                                       enableMiscue,
                                                                       [scenarioId toSpxString]);
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
        NSLog(@"Exception caught in creating SPXPronunciationAssessmentConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];
}


- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                 enableMiscue:(BOOL)enableMiscue
                   scenarioId:(nonnull NSString *)scenarioId
                        error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:referenceText
            gradingSystem:gradingSystem
              granularity:granularity
             enableMiscue:enableMiscue
               scenarioId:scenarioId];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}


- (instancetype)init:(nonnull NSString *)referenceText
       gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
         granularity:(SPXPronunciationAssessmentGranularity)granularity
        enableMiscue:(BOOL)enableMiscue {
    return [self init:referenceText
        gradingSystem:gradingSystem
          granularity:granularity
          enableMiscue:enableMiscue
            scenarioId:@""];
}

- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                 enableMiscue:(BOOL)enableMiscue
                        error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:referenceText
            gradingSystem:gradingSystem
              granularity:granularity
             enableMiscue:enableMiscue];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)init:(nonnull NSString *)referenceText
       gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
         granularity:(SPXPronunciationAssessmentGranularity)granularity {
    return [self init:referenceText
        gradingSystem:gradingSystem
          granularity:granularity
          enableMiscue:false];
}

- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                        error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:referenceText
            gradingSystem:gradingSystem
              granularity:granularity];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (instancetype)init:(nonnull NSString *)referenceText {
    return [self init:referenceText
        gradingSystem:SPXPronunciationAssessmentGradingSystem_FivePoint
          granularity:SPXPronunciationAssessmentGranularity_Phoneme];
}

- (nullable instancetype)init:(nonnull NSString *)referenceText
                        error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self init:referenceText];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}

- (nullable instancetype)initWithJson:(nonnull NSString *)json {
    std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig> configImpl = nil;

    try {
        configImpl = SpeechImpl::PronunciationAssessmentConfig::CreateFromJson([json toSpxString]);
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
        NSLog(@"Exception caught in creating SPXPronunciationAssessmentConfiguration in core");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
    if (configImpl == nullptr)
        return nil;
    return [self initWithImpl:configImpl];
}


- (nullable instancetype)initWithJson:(nonnull NSString *)json
                                error:(NSError * _Nullable * _Nullable)outError {
    try {
        return [self initWithJson:json];
    }
    catch (NSException *exception) {
        NSMutableDictionary *errorDict = [NSMutableDictionary dictionary];
        [errorDict setObject:[NSString stringWithFormat:@"Error: %@", [exception reason]] forKey:NSLocalizedDescriptionKey];
        *outError = [[NSError alloc] initWithDomain:@"SPXErrorDomain"
                                               code:[Util getErrorNumberFromExceptionReason:[exception reason]] userInfo:errorDict];
    }
    return nil;
}


- (void)setReferenceText: (NSString *)text
{
    pronAssessmentConfigImpl->SetReferenceText([text toSpxString]);
}

- (NSString *)referenceText
{
    return [NSString StringWithStdString:pronAssessmentConfigImpl->GetReferenceText()];
}

- (nullable NSString *)toJson {
    if (pronAssessmentConfigImpl == nullptr) {
        NSLog(@"SPXPronunciationAssessmentConfiguration handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXPronunciationAssessmentConfiguration handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        return [NSString StringWithStdString:pronAssessmentConfigImpl->ToJson()];
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
        NSLog(@"Exception caught in toJson");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }

    return nil;
}

- (void)applyToRecognizer:(nonnull SPXRecognizer *)recognizer
{
    if (pronAssessmentConfigImpl == nullptr) {
        NSLog(@"SPXPronunciationAssessmentConfiguration handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXPronunciationAssessmentConfiguration handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    if ([recognizer getHandle] == nullptr) {
        NSLog(@"SPXRecognizer handle is null");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"SPXRecognizer handle is null"
                                                       userInfo:nil];
        [exception raise];
    }

    try {
        pronAssessmentConfigImpl->ApplyTo([recognizer getHandle]);
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
        NSLog(@"Exception caught in applyToRecognizer");
        NSException *exception = [NSException exceptionWithName:@"SPXException"
                                                         reason:@"Runtime exception"
                                                       userInfo:nil];
        [exception raise];
    }
}


- (BOOL)applyToRecognizer:(nonnull SPXRecognizer *)recognizer error:(NSError * _Nullable * _Nullable)outError {
    try {
        [self applyToRecognizer:recognizer];
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

- (instancetype)initWithImpl:(std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig>)configImpl
{
    self = [super init];
    pronAssessmentConfigImpl = configImpl;
    return self;
}

- (std::shared_ptr<SpeechImpl::PronunciationAssessmentConfig>)getHandle
{
    return pronAssessmentConfigImpl;
}

@end
