//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_h
#define recognizer_h

#import <Foundation/Foundation.h>
#import "session_event_args.h"
#import "recognition_event_args.h"

/**
 * Defines the base class Recognizer which mainly contains common event handlers.
 */

@interface Recognizer : NSObject

typedef void (^SessionEventHandlerBlock)(Recognizer *, SessionEventArgs *);
typedef void (^RecognitionEventHandlerBlock)(Recognizer *, RecognitionEventArgs *);

/**
  * The collection or properties and their values defined for this Recognizer.
  */
@property (readonly) id <PropertyCollection> properties;

/**
 * Subscribes to SessionStarted event using block.
 */
- (void)addSessionStartedEventListener:(SessionEventHandlerBlock)eventHandler;

/**
 * Subscribes to SessionStopped event using block.
 */
- (void)addSessionStoppedEventListener:(SessionEventHandlerBlock)eventHandler;

/**
 * Subscribes to SpeechStartDetected event using block.
 */
- (void)addSpeechStartDetectedEventListener:(RecognitionEventHandlerBlock)eventHandler;

/**
 * Subscribes to SpeechEndDetected event using block.
 */
- (void)addSpeechEndDetectedEventListener:(RecognitionEventHandlerBlock)eventHandler;

@end

#endif /* recognizer_h */
