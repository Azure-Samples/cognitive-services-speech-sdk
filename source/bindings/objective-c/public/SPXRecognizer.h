//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import "SPXFoundation.h"
#import "SPXSessionEventArgs.h"
#import "SPXRecognitionEventArgs.h"
#import "SPXPropertyCollection.h"

/**
 * Defines the base class SPXRecognizer which mainly contains common event handlers.
 */

SPX_EXPORT
@interface SPXRecognizer : NSObject

typedef void (^SPXSessionEventHandler)(SPXRecognizer * _Nonnull, SPXSessionEventArgs * _Nonnull);
typedef void (^SPXRecognitionEventHandler)(SPXRecognizer * _Nonnull, SPXRecognitionEventArgs * _Nonnull);

/**
  * The collection of properties and their values defined for this SPXRecognizer.
  */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * Subscribes to the SessionStarted event using the specified handler.
 * @param eventHandler the handler function for this event.
 */
- (void)addSessionStartedEventHandler:(nonnull SPXSessionEventHandler)eventHandler;

/**
 * Subscribes to the SessionStopped event using the specified handler.
 * @param eventHandler the handler function for this event.
 */
- (void)addSessionStoppedEventHandler:(nonnull SPXSessionEventHandler)eventHandler;

/**
 * Subscribes to the SpeechStartDetected event using the specified handler.
 * @param eventHandler the handler function for this event.
 */
- (void)addSpeechStartDetectedEventHandler:(nonnull SPXRecognitionEventHandler)eventHandler;

/**
 * Subscribes to SpeechEndDetected event using the specified handler.
 * @param eventHandler the handler function for this event.
 */
- (void)addSpeechEndDetectedEventHandler:(nonnull SPXRecognitionEventHandler)eventHandler;

@end
