// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

// <code>
#import "AppDelegate.h"
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>


@interface AppDelegate ()
@property (weak) IBOutlet NSWindow *window;
@property (strong) NSTextField *label;
@property (strong) NSButton *button;
@property (strong) SPXSpeechRecognizer *recognizer;
@property (strong) SPXKeywordRecognitionModel *keywordModel;
@property (strong) NSString *keywordText;
@property (assign) BOOL useAsyncAPI;
@property (assign) BOOL isRecognitionActive;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSString *speechKey = @"YourSubscriptionKey";
    NSString *serviceRegion = @"YourServiceRegion";

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    speechConfig.speechRecognitionLanguage = @"en-US";
    //[speechConfig setPropertyTo:@"speechsdk.log" byId:SPXSpeechLogFilename];
    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithMicrophone:nil];
    self.recognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];

    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *keywordModelFile = [bundle pathForResource:@"kws" ofType:@"table"];
    self.keywordModel = [[SPXKeywordRecognitionModel alloc] initFromFile:keywordModelFile];
    self.keywordText = @"computer";
    self.useAsyncAPI = YES;

    __weak typeof(self) weakSelf = self;
    [self.recognizer addRecognizingEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *evt) {
        __strong typeof(self) strongSelf = weakSelf;
        NSLog(@"intermediate recognition result: %@", evt.result.text ?: @"(no result)");
        dispatch_async(dispatch_get_main_queue(), ^{
            [strongSelf.label setStringValue:evt.result.text];
        });
    }];
    [self.recognizer addRecognizedEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *evt) {
        __strong typeof(self) strongSelf = weakSelf;
        NSLog(@"final recognition result: %@", evt.result.text ?: @"(no result)");
        NSString *prefix = (evt.result.reason == SPXResultReason_RecognizedKeyword ? @"KEYWORD:" : @"");
        dispatch_async(dispatch_get_main_queue(), ^{
            [strongSelf.label setStringValue:[NSString stringWithFormat:@"%@%@", prefix, evt.result.text]];
        });
    }];
    [self.recognizer addCanceledEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *evt) {
        __strong typeof(self) strongSelf = weakSelf;
        NSString *cancellationDetails = [NSString stringWithFormat:@"reason=%lu, errorCode=%lu, errorDetails=%@", (unsigned long)evt.reason, (unsigned long)evt.errorCode, evt.errorDetails];
        NSLog(@"canceled: %@", cancellationDetails);
        if (evt.reason == SPXCancellationReason_Error) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [strongSelf.label setStringValue:[NSString stringWithFormat:@"Error: %@", cancellationDetails]];
            });
        }
    }];
    self.isRecognitionActive = NO;

    CGFloat labelWidth = 400.0;
    CGFloat labelHeight = 100.0;
    CGFloat buttonWidth = 200.0;
    CGFloat buttonHeight = 30.0;
    CGFloat verticalSpacing = 20.0;

    NSRect contentViewBounds = self.window.contentView.bounds;
    CGFloat labelX = (contentViewBounds.size.width - labelWidth) / 2;
    CGFloat labelY = (contentViewBounds.size.height - labelHeight - verticalSpacing - buttonHeight) / 2 + verticalSpacing + buttonHeight;
    CGFloat buttonX = (contentViewBounds.size.width - buttonWidth) / 2;
    CGFloat buttonY = labelY - verticalSpacing - buttonHeight;

    self.label = [[NSTextField alloc] initWithFrame:NSMakeRect(labelX, labelY, labelWidth, labelHeight)];
    [self.label setStringValue:@"Press the button to start recognition."];
    [self.label setAlignment:NSTextAlignmentCenter];
    [self.label setBezeled:NO];
    [self.label setDrawsBackground:NO];
    [self.label setEditable:NO];
    [self.window.contentView addSubview:self.label];

    self.button = [NSButton buttonWithTitle:@"Start Recognition" target:nil action:nil];
    [self.button setFrame:NSMakeRect(buttonX, buttonY, buttonWidth, buttonHeight) ];
    [self.button setAutoresizingMask:NSViewMinXMargin | NSViewMaxXMargin | NSViewMinYMargin | NSViewMaxYMargin];
    [self.button setTarget:self];
    [self.button setAction:@selector(buttonPressed:)];
    [self.window.contentView addSubview:self.button];
}

- (void)buttonPressed:(NSButton *)button {
    if (!self.isRecognitionActive) {
        if (self.useAsyncAPI) {
            [self.recognizer startKeywordRecognitionAsync:self.keywordModel completionHandler:^{
                NSLog(@"started listening");
            }];
        } else {
            [self.recognizer startKeywordRecognition:self.keywordModel];
            NSLog(@"started listening");
        }
        self.isRecognitionActive = YES;
        [self.button setTitle:@"Stop Recognition"];
        [self.label setStringValue:[NSString stringWithFormat:@"Say something starting with '%@', press the button to stop...", self.keywordText]];
    } else {
        if (self.useAsyncAPI) {
            [self.recognizer stopKeywordRecognitionAsync:/* completionHandler */^{
                NSLog(@"stopped listening");
            }];
        } else {
            [self.recognizer stopKeywordRecognition];
            NSLog(@"stopped listening");
        }
        self.isRecognitionActive = NO;
        [self.button setTitle:@"Start Recognition"];
        [self.label setStringValue:@"Press the button to start recognition."];
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

@end
// </code>
