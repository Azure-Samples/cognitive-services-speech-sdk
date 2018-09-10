//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognizer_h
#define recognizer_h

#import <Foundation/Foundation.h>
#import "session_event_args.h"
#import "recognition_event_args.h"
#import "recognition_error_event_args.h"

@interface Recognizer : NSObject

typedef void (^SessionEventHandlerBlock)(Recognizer *, SessionEventArgs *);
typedef void (^RecognitionEventHandlerBlock)(Recognizer *, RecognitionEventArgs *);
typedef void (^ErrorEventHandlerBlock)(Recognizer *, RecognitionErrorEventArgs *);

- (void)addSessionEventListener:(SessionEventHandlerBlock)eventHandler;
- (void)addRecognitionEventListener:(RecognitionEventHandlerBlock)eventHandler;
- (void)addErrorEventListener:(ErrorEventHandlerBlock)eventHandler;


@end

#endif /* recognizer_h */
