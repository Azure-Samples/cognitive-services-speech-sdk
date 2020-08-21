//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXAudioConfiguration.h"
#import "SPXPropertyCollection.h"
#import "SPXRecognizer.h"
#import "SPXKeywordRecognitionModel.h"
#import "SPXKeywordRecognitionResult.h"
#import "SPXKeywordRecognitionEventArgs.h"

/**
 * Recognizer type that is specialized to only handle keyword activation.
 *
 * Added in version 1.15.0.
 * 
 */
SPX_EXPORT
@interface SPXKeywordRecognizer : NSObject

typedef void (^SPXKeywordRecognitionEventHandler)(SPXKeywordRecognizer * _Nonnull, SPXKeywordRecognitionEventArgs * _Nonnull);
typedef void (^SPXKeywordRecognitionCanceledEventHandler)(SPXKeywordRecognizer * _Nonnull, SPXKeywordRecognitionCanceledEventArgs * _Nonnull);

/**
 * The collection of properties and their values defined for this SPXKeywordRecognizer.
 */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * Initializes a new instance of keyword recognizer using the specified audio configuration.
 * 
 * @param audioConfiguration audio configuration.
 * @return an keyword recognizer.
 */
- (nullable instancetype)init:(nonnull SPXAudioConfiguration *)audioConfiguration
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Initializes a new instance of keyword recognizer using the specified audio configuration.
 *
 * @param audioConfiguration audio configuration.
 * @param outError error information.
 * @return an keyword recognizer.
 */
- (nullable instancetype)init:(nonnull SPXAudioConfiguration *)audioConfiguration error:(NSError * _Nullable * _Nullable)outError;

/**
 * Starts a keyword recognition session. This session will last until the first keyword is recognized. 
 * When this happens, a Recognized event will be raised and the session will end. To rearm the keyword, 
 * the method needs to be called again after the event is emitted.
 *
 * Note that if no keyword is detected in the input, the task will never resolve (unless stopRecognitionAsync method is called).
 *
 * @param resultReceivedHandler the block function to be called when the keyword has been recognized.
 * @param keywordModel the keyword recognition model.
 */
- (void)recognizeOnceAsync:(nonnull void (^)(SPXKeywordRecognitionResult * _Nonnull)) resultReceivedHandler keywordModel:(nonnull SPXKeywordRecognitionModel *)keywordModel
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Starts a keyword recognition session. This session will last until the first keyword is recognized. 
 * When this happens, a Recognized event will be raised and the session will end. To rearm the keyword, 
 * the method needs to be called again after the event is emitted.
 *
 * Note that if no keyword is detected in the input, the task will never resolve (unless stopRecognitionAsync method is called).
 * 
 * @param resultReceivedHandler the block function to be called when the keyword has been recognized.
 * @param keywordModel the keyword recognition model.
 * @param outError error information.
 */
- (BOOL)recognizeOnceAsync:(nonnull void (^)(SPXKeywordRecognitionResult * _Nonnull)) resultReceivedHandler keywordModel:(nonnull SPXKeywordRecognitionModel *)keywordModel error:(NSError * _Nullable * _Nullable)outError;

/**
 * Stops a currently active keyword recognition session.
 *
 * @param completedHandler the block function to be called when async operation has been completed.
 */
- (void)stopRecognitionAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler
NS_SWIFT_UNAVAILABLE("Use the method with Swift-compatible error handling.");

/**
 * Stops a currently active keyword recognition session.
 * 
 * @param completedHandler the block function to be called when async operation has been completed.
 * @param outError error information.
 */
- (BOOL)stopRecognitionAsync:(nonnull void (^)(BOOL stopped, NSError * _Nullable))completedHandler error:(NSError * _Nullable * _Nullable)outError;

/**
 * Subscribes to the Recognized event which indicates that a final result has been recognized.
 */
- (void)addRecognizedEventHandler:(nonnull SPXKeywordRecognitionEventHandler)eventHandler;

/**
 * Subscribes to the Canceled event which indicates that an error occurred during recognition.
 */
- (void)addCanceledEventHandler:(nonnull SPXKeywordRecognitionCanceledEventHandler)eventHandler;

@end
