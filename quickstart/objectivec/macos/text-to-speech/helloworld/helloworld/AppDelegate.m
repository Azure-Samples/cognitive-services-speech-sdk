//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
#import "AppDelegate.h"
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@property (strong) NSButton *button;
@property (strong) NSTextField *textField;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    CGFloat buttonWidth = 200.0;
    CGFloat buttonHeight = 32.0;
    CGFloat textWidth = 200.0;
    CGFloat textHeight = 64.0;
    CGFloat verticalSpacing = 20.0;

    NSRect contentViewBounds = self.window.contentView.bounds;
    CGFloat buttonX = (contentViewBounds.size.width - buttonWidth) / 2;
    CGFloat buttonY = (contentViewBounds.size.height - buttonHeight) / 2 - verticalSpacing;
    CGFloat textX = (contentViewBounds.size.width - textWidth) / 2;
    CGFloat textY = (contentViewBounds.size.height - textHeight) / 2 + textHeight + verticalSpacing;

    self.button = [NSButton buttonWithTitle:@"Synthesize" target:nil action:nil];
    [self.button setFrame:NSMakeRect(buttonX, buttonY, buttonWidth, buttonHeight) ];
    [self.button setAutoresizingMask:NSViewMinXMargin | NSViewMaxXMargin | NSViewMinYMargin | NSViewMaxYMargin];
    [self.button setTarget:self];
    [self.button setAction:@selector(buttonPressed:)];
    [self.window.contentView addSubview:self.button];

    self.textField = [[NSTextField alloc] initWithFrame:NSMakeRect(textX, textY, textWidth, textHeight)];
    [self.textField setPlaceholderString:@"Text something to synthesize..."];
    [self.window.contentView addSubview:self.textField];
}

- (void)buttonPressed:(NSButton *)button {
    // Creates an instance of a speech config with specified subscription key and service region.
    // Replace with your own subscription key and service region (e.g., "westus").
    NSString *speechKey = @"YourSubscriptionKey";
    NSString *serviceRegion = @"YourServiceRegion";
    
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    SPXSpeechSynthesizer *speechSynthesizer = [[SPXSpeechSynthesizer alloc] init:speechConfig];
    
    NSLog(@"Start synthesizing...");
    
    SPXSpeechSynthesisResult *speechResult = [speechSynthesizer speakText:[self.textField stringValue]];
    
    // Checks result.
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXSpeechSynthesisCancellationDetails *details = [[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:speechResult];
        NSLog(@"Speech synthesis was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
    } else if (SPXResultReason_SynthesizingAudioCompleted == speechResult.reason) {
        NSLog(@"Speech synthesis was completed");
    } else {
        NSLog(@"There was an error.");
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return true;
}

@end
// </code>
