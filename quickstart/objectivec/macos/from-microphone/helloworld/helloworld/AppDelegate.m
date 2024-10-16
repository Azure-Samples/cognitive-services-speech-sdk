// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

// <code>
#import "AppDelegate.h"
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>


@interface AppDelegate ()
@property (weak) IBOutlet NSWindow *window;
@property (strong) NSButton *button;
@property (strong) NSTextField *label;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    CGFloat buttonWidth = 200.0;
    CGFloat buttonHeight = 32.0;
    CGFloat labelWidth = 200.0;
    CGFloat labelHeight = 32.0;
    CGFloat verticalSpacing = 20.0;

    NSRect contentViewBounds = self.window.contentView.bounds;
    CGFloat buttonX = (contentViewBounds.size.width - buttonWidth) / 2;
    CGFloat buttonY = (contentViewBounds.size.height - buttonHeight) / 2 - verticalSpacing;
    CGFloat labelX = (contentViewBounds.size.width - labelWidth) / 2;
    CGFloat labelY = (contentViewBounds.size.height - labelHeight) / 2 + labelHeight + verticalSpacing;

    self.button = [NSButton buttonWithTitle:@"Start Recognition" target:nil action:nil];
    [self.button setFrame:NSMakeRect(buttonX, buttonY, buttonWidth, buttonHeight) ];
    [self.button setAutoresizingMask:NSViewMinXMargin | NSViewMaxXMargin | NSViewMinYMargin | NSViewMaxYMargin];
    [self.button setTarget:self];
    [self.button setAction:@selector(buttonPressed:)];
    [self.window.contentView addSubview:self.button];

    self.label = [[NSTextField alloc] initWithFrame:NSMakeRect(labelX, labelY, labelWidth, labelHeight)];
    [self.label setStringValue:@"Press Button!"];
    [self.label setAlignment:NSTextAlignmentCenter];
    [self.label setBezeled:NO];
    [self.label setDrawsBackground:NO];
    [self.label setEditable:NO];
    [self.window.contentView addSubview:self.label];
}

- (void)buttonPressed:(NSButton *)button {
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    NSString *speechKey = @"YourSubscriptionKey";
    NSString *serviceRegion = @"YourServiceRegion";

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithMicrophone:nil];
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:audioConfig];

    NSLog(@"Say something...");

    // Starts speech recognition, and returns after a single utterance is recognized. The end of a
    // single utterance is determined by listening for silence at the end or until a maximum of about 30
    // seconds of audio is processed.  The task returns the recognition text as result.
    // Note: Since recognizeOnce() returns only a single utterance, it is suitable only for single
    // shot recognition like command or query.
    // For long-running multi-utterance recognition, use startContinuousRecognitionAsync() instead.
    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];

    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self.label setStringValue:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        [self.label setStringValue:(speechResult.text)];
    } else if (SPXResultReason_NoMatch == speechResult.reason) {
        SPXNoMatchDetails *details = [[SPXNoMatchDetails alloc] initFromNoMatchRecognitionResult:speechResult];
        NSLog(@"No speech recognized, reason: %lu", details.reason);
        NSString *noMatchReason;
        if (details.reason == SPXNoMatchReason_InitialBabbleTimeout || details.reason == SPXNoMatchReason_EndSilenceTimeout) {
            noMatchReason = @"Silence Timeout";
        } else {
            noMatchReason = @"Not Recognized";
        }
        [self.label setStringValue:(noMatchReason)];
    } else {
        NSLog(@"There was an error.");
        [self.label setStringValue:(@"Speech Recognition Error")];
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

@end
// </code>
