//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface AudioRecorder : NSObject

- (instancetype)initWithPushStream:(SPXPushAudioInputStream *)stream;

@property (nonatomic, assign, readonly) BOOL isRunning;

- (void)record;

- (void)stop;

@end

