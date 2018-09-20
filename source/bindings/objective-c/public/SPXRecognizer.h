//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#import <Foundation/Foundation.h>
#import "SPXSessionEventArgs.h"
#import "SPXRecognitionEventArgs.h"

/**
 * Defines the base class SPXRecognizer which mainly contains common event handlers.
 */

@interface SPXRecognizer : NSObject

typedef void (^SPXSessionEventHandler)(SPXRecognizer * _Nonnull, SPXSessionEventArgs * _Nonnull);
typedef void (^SPXRecognitionEventHandler)(SPXRecognizer * _Nonnull, SPXRecognitionEventArgs * _Nonnull);

/**
  * The collection or properties and their values defined for this SPXRecognizer.
  */
@property (readonly, nullable)id <SPXPropertyCollection> properties;

/**
 * Subscribes to SessionStarted event using block.
 */
- (void)addSessionStartedEventHandler:(nonnull SPXSessionEventHandler)eventHandler;

/**
 * Subscribes to SessionStopped event using block.
 */
- (void)addSessionStoppedEventHandler:(nonnull SPXSessionEventHandler)eventHandler;

/**
 * Subscribes to SpeechStartDetected event using block.
 */
- (void)addSpeechStartDetectedEventHandler:(nonnull SPXRecognitionEventHandler)eventHandler;

/**
 * Subscribes to SpeechEndDetected event using block.
 */
- (void)addSpeechEndDetectedEventHandler:(nonnull SPXRecognitionEventHandler)eventHandler;

@end
