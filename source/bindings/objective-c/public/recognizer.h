//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_h
#define recognizer_h

#import <Foundation/Foundation.h>
#import "session_event_args.h"
#import "recognition_event_args.h"

@interface Recognizer : NSObject

typedef void (^SessionEventHandlerBlock)(Recognizer *, SessionEventArgs *);
typedef void (^RecognitionEventHandlerBlock)(Recognizer *, RecognitionEventArgs *);

- (void)addSessionEventListener:(SessionEventHandlerBlock)eventHandler;
- (void)removeSessionEventListener:(SessionEventHandlerBlock)eventHandler;

- (void)addRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler;
- (void)removeRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler;

@end

#endif /* recognizer_h */
