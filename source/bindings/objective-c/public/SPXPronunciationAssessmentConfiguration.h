//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSpeechEnums.h"
#import "SPXRecognizer.h"

/**
 * Represents pronunciation assessment configuration
 * Added in version 1.14.0
 */
SPX_EXPORT
@interface SPXPronunciationAssessmentConfiguration : NSObject

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 * @param gradingSystem The point system for score calibration.
 * @param granularity The evaluation granularity.
 * @param enableMiscue If enables miscue calculation.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                 enableMiscue:(BOOL)enableMiscue
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 * @param gradingSystem The point system for score calibration.
 * @param granularity The evaluation granularity.
 * @param enableMiscue If enables miscue calculation.
 * @param outError error information.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                 enableMiscue:(BOOL)enableMiscue
                        error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 * @param gradingSystem The point system for score calibration.
 * @param granularity The evaluation granularity.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 * @param gradingSystem The point system for score calibration.
 * @param granularity The evaluation granularity.
 * @param outError error information.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
                gradingSystem:(SPXPronunciationAssessmentGradingSystem)gradingSystem
                  granularity:(SPXPronunciationAssessmentGranularity)granularity
                        error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration.
 * For the parameters details, see
 * https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-speech-to-text#pronunciation-assessment-parameters
 *
 * @param referenceText The reference text.
 * @param outError error information.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)init:(nonnull NSString *)referenceText
                        error:(NSError * _Nullable * _Nullable)outError;

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration using json string.
 *
 * @param json the json string.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)initWithJson:(nonnull NSString *)json
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes an instance of the SPXPronunciationAssessmentConfiguration using json string.
 *
 * @param json the json string.
 * @param outError error information.
 *
 * @return an instance of pronunciation assessment configuration.
 */
- (nullable instancetype)initWithJson:(nonnull NSString *)json
                                error:(NSError * _Nullable * _Nullable)outError;

/**
 * The reference text.
 */
@property (nonatomic, copy, nullable)NSString *referenceText;

/**
 * Scenario ID, which is a GUID indicating a customized point system.
 */
@property (nonatomic, copy, nullable)NSString *scenarioId;

/**
 * Gets the json string of pronunciation assessment parameters.
 *
 * @return json string of pronunciation assessment parameters.
 */
- (nullable NSString *)toJson;

/**
 * Applies the settings in this config to a recognizer.
 *
 * @param recognizer The target recognizer.
 */
- (void)applyToRecognizer:(nonnull SPXRecognizer *)recognizer
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");


/**
 * Applies the settings in this config to a recognizer.
 *
 * @param recognizer The target recognizer.
 * @param outError error information.
 */
- (BOOL)applyToRecognizer:(nonnull SPXRecognizer *)recognizer error:(NSError * _Nullable * _Nullable)outError;

@end
