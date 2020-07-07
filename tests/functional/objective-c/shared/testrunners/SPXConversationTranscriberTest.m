//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXConversationTranscriberTest : XCTestCase {
    NSString *singleUtteranceTextEnglish;
    NSString *conversationTextEnglish;

    NSString *singleUtteranceFileName;
    NSString *conversationFileName;
    NSString *person1EnrollmentFileName;
    NSString *person2EnrollmentFileName;
    NSString *person1VoiceSignature;
    NSString *person2VoiceSignature;

    NSMutableDictionary *result;
    NSPredicate *sessionStoppedCountPred;
    double timeoutInSeconds;
}

@property (nonatomic, assign) NSString* speechKey;
@property (nonatomic, assign) NSString* serviceRegion;
@property (nonatomic, retain) SPXSpeechConfiguration* speechConfig;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@property (nonatomic, retain) SPXConversationTranscriber* conversationTranscriber;
@property (nonatomic, retain) SPXConversation* conversation;
@property (nonatomic, retain) SPXParticipant* participant1;
@property (nonatomic, retain) SPXParticipant* participant2;
@property (nonatomic, retain) SPXParticipant* participant3;
@property (nonatomic, retain) SPXUser* user1;

- (void)setSpeechConfig;

@end

@implementation SPXConversationTranscriberTest

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    conversationFileName = @"katiesteve";
    conversationTextEnglish = @"Good morning Steve";
    person1EnrollmentFileName = @"enrollment_audio_katie";
    person2EnrollmentFileName = @"enrollment_audio_steve";
    person1VoiceSignature = @"{ \"Version\": 0, \"Tag\": \"VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=\", \"Data\": \"BhRRgDCrg6ij5fylg5Jpf5ZW/o/uDWi199DYBbfL1Qdspj77qiuvsVKzG2g5Z9jxKtfdwtkKtaDxog9O6pGD7Ot/8mM1jUtt6LKNz4H9EFvznV/dlFk2Oisg8ZKx/RBlNFMYJkQJnxT/zLfhNWiIF5Y97jH4sgRh2orDg6/567FGktAgcESAbiDx1e7tf0TTLdwijw4p1vJ3qJ2cSCdNbXE9KeUd8sClQLDheCPo+et3rMs5W+Rju3W1SJE6ru9gAoH88CyAfI80+ysAecH3GPJYM+j1uhvmWoKIrSfS40BYOe6AUgLNb3a4Pue4oGAmuAyWfwpP1uezleSanpJc73HT91n2UsaHrQj5eK6uuBCjwmu+JI3FT+Vo6JhAARHecdb70U1wyW9vv5t0Q3tV1WNiN/30qSVydDtyrSVpgBiIwlj41Ua22JJCOMrPl7NKLnFmeZ4Hi4aIKoHAxDvrApteL60sxLX/ADAtYCB3Y6iagDyR1IOsIlbaPhL0rQDnC/0z65k7BDekietFNzvvPVoIwJ26GHrXFYYwZe3alVvCsXTpZGBknvSiaCalrixnyGqYo0nG/cd/LodEEIht/PWoFkNlbABqHMbToeI/6j+ICKuVJgTDtcsDQiWKSvrQp9kTSv+cF3LyPVkbks0JvbQhj4AkAv7Rvymgnsu6o8gGyxTX0kxujqCMPCzgFrVd\"}";
    person2VoiceSignature = @"{ \"Version\": 0, \"Tag\": \"HbIvzbfAWjeR/3R+WvUEoeid1AbDaHNOMWItgs7mTxc=\", \"Data\": \"DizY04Z7PH/sYu2Yw2EcL4Mvj1GnEDOWJ/DhXHGdQJsQ8/zDc13z1cwllbEo5OSr3oGoKEHLV95OUA6PgksZzvTkf42iOFEv3yifUNfYkZuIzStZoDxWu1H1BoFBejqzSpCYyvqLwilWOyUeMn+z+E4+zXjqHUCyYJ/xf0C3+58kCbmyA55yj7YZ6OtMVyFmfT2GLiXr4YshUB14dgwl3Y08SRNavnG+/QOs+ixf3UoZ6BC1VZcVQnC2tn2FB+8v6ehnIOTQedo++6RWIB0RYmQ8VaEeI0E4hkpA1OxQ9f2gBVtw3KZXWSWBz8sXig2igpwMsQoFRmmIOGsu+p6tM8/OThQpARZ7OyAxsurzmaSGZAaXYt0YwMdIIXKeDBF6/KnUyw+NNzku1875u2Fde/bxgVvCOwhrLPPuu/RZUeAkwVQge7nKYNW5YjDcz8mfg4LfqWEGOVCcmf2IitQtcIEjY3MwLVNvsAB6GT2es1/1QieCfQKy/Tdu8IUfEvekwSCxSlWhfVrLjRhGeWa9idCjsngQbNkqYUNdnIlidkn2DC4BavSTYXR5lVxV4SR/Vvj8h4N5nP/URPDhkzl7n7Tqd4CGFZDzZzAr7yRo3PeUBX0CmdrKLW3+GIXAdvpFAx592pB0ySCv5qBFhJNErEINawfGcmeWZSORxJg1u+agj51zfTdrHZeugFcMs6Be\"}";
    singleUtteranceFileName = @"whatstheweatherlike";
    singleUtteranceTextEnglish = @"What's the weather like?";
    
    [self setSpeechConfig];
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource: singleUtteranceFileName ofType:@"wav"];
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:targetFile];
    self.conversationTranscriber = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    
    [self setEventHandlers];
}

- (void)setSpeechConfig {
    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"conversationTranscriberPPEKey"];
    self.speechConfig = [[SPXSpeechConfiguration alloc] initWithEndpoint:@"wss://westus2.online.princetondev.customspeech.ai/recognition/onlinemeeting/v1" subscription:self.speechKey];
    [self.speechConfig setPropertyTo:@"true" byName:@"ConversationTranscriptionInRoomAndOnline"];
    
}

- (void)setEventHandlers {
    result = [NSMutableDictionary new];
    [result setObject:@0 forKey:@"finalResultCount"];
    [result setObject:@0 forKey:@"sessionStoppedCount"];
    [result setObject:@"" forKey:@"finalText"];

    __unsafe_unretained typeof(self) weakSelf = self;

    [self.conversationTranscriber addTranscribedEventHandler: ^ (SPXConversationTranscriber *transcriber, SPXConversationTranscriptionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, transcription result:%@. transcription userId:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text,
              eventArgs.result.userId,(long)eventArgs.result.reason,
              eventArgs.result.offset, eventArgs.result.duration,
              eventArgs.result.resultId);
        // Do not store empty results
        if ([eventArgs.result.text length] != 0) {
            [weakSelf->result setObject:eventArgs.result.text forKey: @"finalText"];
            [weakSelf->result setObject:eventArgs.result.userId forKey: @"userId"];
            [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"finalResultCount"] integerValue] + 1] forKey:@"finalResultCount"];
        }
    }];

    [self.conversationTranscriber addSessionStartedEventHandler: ^ (SPXRecognizer *transcriber, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session started event. SessionId: %@", eventArgs.sessionId);
    }];

    [self.conversationTranscriber addSessionStoppedEventHandler: ^ (SPXRecognizer *transcriber, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        [weakSelf->result setObject:[NSNumber numberWithLong:[[weakSelf->result objectForKey:@"sessionStoppedCount"] integerValue] + 1] forKey:@"sessionStoppedCount"];
    }];
    
    sessionStoppedCountPred = [NSPredicate predicateWithBlock: ^BOOL (id  _Nullable evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings){
        return [[evaluatedObject valueForKey:@"sessionStoppedCount"] isEqualToNumber:@1];
    }];
}

- (void)tearDown {
    [super tearDown];
}

- (void)testCreateConversationTranscriber {
    SPXConversationTranscriber* conversationTranscriber = [[SPXConversationTranscriber alloc] init];
    XCTAssertNotNil(conversationTranscriber);

    SPXConversationTranscriber* conversationTranscriber2 = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    XCTAssertNotNil(conversationTranscriber2);
}

- (void)testCreateConversationIdWithChinese {
    SPXConversationTranscriber* conversationTranscriber = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    XCTAssertNotNil(conversationTranscriber);
    
    NSString* conversationId = @"的";
    SPXConversation* conversation = [[SPXConversation alloc] init:self.speechConfig conversationId:conversationId];
    XCTAssertNotNil(conversation);
    NSString* gotId = conversation.conversationId;
    XCTAssertEqualObjects(conversationId, gotId);
}

- (void)testCreateConversationId {
    SPXConversationTranscriber* conversationTranscriber = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    XCTAssertNotNil(conversationTranscriber);
    
    NSString* conversationId = @"123 456";
    SPXConversation* conversation = [[SPXConversation alloc] init:self.speechConfig conversationId:conversationId];
    XCTAssertNotNil(conversation);
    NSString* gotId = conversation.conversationId;
    XCTAssertEqualObjects(conversationId, gotId);
}

- (void)testCreateUserValidInput {
    NSString* userId = @"xyz@myexample.com";
    SPXUser* user = [[SPXUser alloc] initFromUserId:userId];
    XCTAssertNotNil(user);
    XCTAssertEqualObjects(userId, user.userId);
}

- (void)testCreateUserInvalidInput {
    BOOL exception = NO;
    NSString* userId = @"";
    @try {
        SPXUser* user = [[SPXUser alloc] initFromUserId:userId];
        XCTAssertNotNil(user);
    }
    @catch (NSException* e) {
        NSLog(@"%@", e.reason);
        exception = YES;
    }
    XCTAssertEqual(exception, YES);
}

- (void)testCreateParticipantValidInput {
    BOOL exception = NO;
    @try {
        SPXParticipant* participant = [[SPXParticipant alloc] initFrom:@"xyz@example.com" preferredLanguage:@"zh-cn" voiceSignature:person1VoiceSignature];
        XCTAssertNotNil(participant);
        SPXParticipant* participant2 = [[SPXParticipant alloc] initFrom:@"xyz@example.com" preferredLanguage:@"" voiceSignature:person1VoiceSignature];
        XCTAssertNotNil(participant2);
        SPXParticipant* participant3 = [[SPXParticipant alloc] initFrom:@"xyz@example.com" preferredLanguage:@"" voiceSignature:@""];
        XCTAssertNotNil(participant3);
    }
    @catch (NSException* e) {
        NSLog(@"%@", e.reason);
        exception = YES;
    }
    XCTAssertEqual(exception, NO);
}

- (void)testCreateParticipantInValidInput {
    BOOL exception = NO;
    @try {
        SPXParticipant* participant = [[SPXParticipant alloc] initFrom:@"" preferredLanguage:@"zh-cn" voiceSignature:person1VoiceSignature];
        XCTAssertNotNil(participant);
    }
    @catch (NSException* e) {
        NSLog(@"%@", e.reason);
        exception = YES;
    }
    XCTAssertEqual(exception, YES);
    
    exception = NO;
    @try {
        SPXParticipant* participant = [[SPXParticipant alloc] initFrom:@"xyz@example.com" preferredLanguage:@"zh-cn" voiceSignature:@"1.1, 2.2"];
        XCTAssertNotNil(participant);
    }
    @catch (NSException* e) {
        NSLog(@"%@", e.reason);
        exception = YES;
    }
    XCTAssertEqual(exception, YES);
}

- (void)testAddAndRemoveParticipantsFromConversation {
    NSString* conversationId = @"meeting4321Id";
    self.conversation = [[SPXConversation alloc] init:self.speechConfig conversationId:conversationId];
    XCTAssertNotNil(self.conversation);
    
    NSLog(@"conversation created successfully");

    [self.conversation.properties setPropertyTo:@"MTUri_value" byName:@"MTUri"];
    [self.conversation.properties setPropertyTo:@"true" byName:@"DifferenciateGuestSpeakers"];
    [self.conversation.properties setPropertyTo:@"iCalUId_value" byName:@"iCalUId"];
    [self.conversation.properties setPropertyTo:@"callId_value" byName:@"callId"];
    [self.conversation.properties setPropertyTo:@"organizer_value" byName:@"organizer"];
    [self.conversation.properties setPropertyTo:@"on" byName:@"audiorecording"];
    [self.conversation.properties setPropertyTo:@"Threadid_value" byName:@"Threadid"];
    [self.conversation.properties setPropertyTo:@"Organizer_Mri_value" byName:@"Organizer_Mri"];
        
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversationTranscriber joinConversationAsync: ^(BOOL joined, NSError * err) {
        XCTAssertEqual(joined, YES);
        XCTAssertNil(err);
        NSLog(@"Joined to conversation successfully");
        [expectation fulfill];
    } conversation:self.conversation];

    NSLog(@"Waiting for joining");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    expectation = [self expectationWithDescription:@"async request"];

    @try {
        [self.conversation removeParticipantAsync: ^ (BOOL removed, NSError * err) {
            XCTAssertEqual(removed, NO);
            XCTAssertNotNil(err);
            NSLog(@"Participant not removed from conversation successfully");
            [expectation fulfill];
        } userId:@"DoNotExist"];
        
        NSLog(@"Waiting for removing 1");
        [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    }
    @catch(NSException *e) {
        NSLog(@"Exceptation in synchronous part, not failure");
    }
    
    expectation = [self expectationWithDescription:@"async request"];
    
    __block SPXParticipant *asyncParticipantResult = nil;
    [self.conversation addParticipantAsync: ^ (SPXParticipant * participant, NSError * err) {
        asyncParticipantResult = participant;
        NSLog(@"Participant added successfully");
        [expectation fulfill];
    } userId:@"UserIdForParticipant1"];

    NSLog(@"Waiting for adding 1");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    self.participant1 = asyncParticipantResult;
    XCTAssertNotNil(asyncParticipantResult);
    
    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversation removeParticipantAsync: ^ (BOOL removed, NSError * err) {
        XCTAssertEqual(removed, YES);
        XCTAssertNil(err);
        NSLog(@"Participant1 removed from conversation successfully");
        [expectation fulfill];
    } userId:@"UserIdForParticipant1"];

    NSLog(@"Waiting for removing 1");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    expectation = [self expectationWithDescription:@"async request"];
    
    self.participant2 = [[SPXParticipant alloc] initFrom:@"UserIdForParticipant2" preferredLanguage:@"en-US" voiceSignature:person2VoiceSignature];
    
    [self.conversation addParticipantAsync: ^ (SPXParticipant * participant, NSError * err) {
        if (err == nil) {
            NSLog(@"Participant2 added successfully");
            [expectation fulfill];
        }
    } participant:self.participant2];

    NSLog(@"Waiting for adding 2");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertNotNil(self.participant2);

    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversation removeParticipantAsync: ^ (BOOL removed, NSError * err) {
        XCTAssertEqual(removed, YES);
        XCTAssertNil(err);
        NSLog(@"Participant2 removed from conversation successfully");
        [expectation fulfill];
    } participant:self.participant2];

    NSLog(@"Waiting for removing 2");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    expectation = [self expectationWithDescription:@"async request"];
    
    self.user1 = [[SPXUser alloc] initFromUserId:@"UserId1"];
    [self.conversation addParticipantAsync: ^ (SPXUser * user, NSError * err) {
        if (err == nil) {
            NSLog(@"Participant3 added successfully");
            [expectation fulfill];
        }
    } user:self.user1];

    NSLog(@"Waiting for adding 3");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertNotNil(self.user1);

    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversation removeParticipantAsync: ^ (BOOL removed, NSError * err) {
        XCTAssertEqual(removed, YES);
        XCTAssertNil(err);
        NSLog(@"Participant3 removed from conversation successfully");
        [expectation fulfill];
    } user:self.user1];

    NSLog(@"Waiting for removing 3");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversation addParticipantAsync: ^ (SPXParticipant * participant, NSError * err) {
        asyncParticipantResult = participant;
        NSLog(@"Participant4 added successfully");
        [expectation fulfill];
    } userId:@"UserIdForParticipant4"];

    NSLog(@"Waiting for adding 4");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    self.participant3 = asyncParticipantResult;
    
    __block BOOL asyncStarted = false;
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranscriber startTranscribingAsync: ^ (BOOL started, NSError * err) {
        asyncStarted = started;
        NSLog(@"Conversation transcribing started successfully");
        [expectation fulfill];
    }];
    
    NSLog(@"Waiting for starting");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertTrue(asyncStarted);

    NSLog(@"Waiting for session stopped event");
    [self expectationForPredicate:sessionStoppedCountPred evaluatedWithObject:result handler:nil];
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertNotNil(self.participant3);
    
    __block BOOL asyncStopped = false;
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranscriber stopTranscribingAsync: ^ (BOOL stopped, NSError * err) {
        asyncStopped = stopped;
        NSLog(@"Conversation transcribing stopped successfully");
        [expectation fulfill];
    }];
    
    NSLog(@"Waiting for stopping");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertTrue(asyncStopped);

    __block BOOL asyncLeft = false;
    expectation = [self expectationWithDescription:@"async request"];
    [self.conversationTranscriber leaveConversationAsync:^(BOOL left, NSError * err) {
        asyncLeft = left;
        NSLog(@"Left from conversation successfully");
        [expectation fulfill];
    }];
    
    NSLog(@"Waiting for leaving");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    XCTAssertTrue(asyncLeft);
}

@end

@interface SPXConversationTranscriberPushStreamTest : SPXConversationTranscriberTest {
    AVAudioFile *audioFile;
    SPXPushAudioInputStream* stream;
    
    NSUInteger sampleRate;
}

- (void)setPushStreamAudio;

@end

@implementation SPXConversationTranscriberPushStreamTest

- (void)setPushStreamAudio {
    sampleRate = 16000;
    
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:singleUtteranceFileName ofType:@"wav"];

    NSURL *targetUrl = [NSURL URLWithString:targetFile];
    NSError *error = nil;
    audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];

    if (error) {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(self->sampleRate == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    self->stream = [[SPXPushAudioInputStream alloc] init];
    self.audioConfig  = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
    self.conversationTranscriber = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    [self setEventHandlers];
}

- (void)testPushStreamConversation {
    [self setPushStreamAudio];
    
    NSString* conversationId = @"meeting4321Id";
    self.conversation = [[SPXConversation alloc] init:self.speechConfig conversationId:conversationId];
    XCTAssertNotNil(self.conversation);

    [self.conversation.properties setPropertyTo:@"040000008200E00074C5B7101A82E008000000001003D722197CD4010000000000000000100000009E970FDF583F9D4FB999E607891A2F66" byName:@"iCalUId"];
    
    NSLog(@"conversation created successfully");
    
    XCTestExpectation *expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversationTranscriber joinConversationAsync: ^(BOOL joined, NSError * err) {
        if (joined) {
            NSLog(@"Joined to conversation successfully");
        }
        [expectation fulfill];
    } conversation:self.conversation];

    NSLog(@"Waiting for conversation joining");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];

    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversationTranscriber startTranscribingAsync:^(BOOL started, NSError * err) {
        NSLog(@"Transcribing started");
        [expectation fulfill];
    }];

    NSLog(@"Waiting for conversation starting");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    const AVAudioFrameCount nBytesToRead = 5000;
    const NSInteger bytesPerFrame = self->audioFile.fileFormat.streamDescription->mBytesPerFrame;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:self->audioFile.fileFormat frameCapacity:nBytesToRead / bytesPerFrame];

    NSAssert(1 == buffer.stride, @"only one channel allowed");
    NSAssert(nil != buffer.int16ChannelData, @"assure correct format");

    // push data to stream;
    while (1) {
        NSError *bufferError = nil;
        bool success = [self->audioFile readIntoBuffer:buffer error:&bufferError];
        if (!success) {
            NSLog(@"Read error on stream: %@", bufferError);
            [self->stream close];
            break;
        }
        else {
            NSInteger nBytesRead = [buffer frameLength] * bytesPerFrame;
            if (0 == nBytesRead) {
                [self->stream close];
                break;
            }
            NSLog(@"Read %d bytes from file", (int)nBytesRead);
            NSData *data = [NSData dataWithBytesNoCopy:buffer.int16ChannelData[0] length:nBytesRead freeWhenDone:NO];
            NSLog(@"%d bytes data returned", (int)[data length]);
            [self->stream write:data];
            NSLog(@"Wrote %d bytes to stream", (int)[data length]);
        }
        [NSThread sleepForTimeInterval:0.05f];
    }

    [self expectationForPredicate:self->sessionStoppedCountPred evaluatedWithObject:self->result handler:nil];
    [self waitForExpectationsWithTimeout:self->timeoutInSeconds handler:nil];

    expectation = [self expectationWithDescription:@"async request"];
    
    [self.conversationTranscriber stopTranscribingAsync: ^ (BOOL stopped, NSError * err) {
        NSLog(@"Transcribing stopped");
        [expectation fulfill];
    }];

    NSLog(@"Waiting for conversation stopping");
    [self waitForExpectationsWithTimeout:timeoutInSeconds handler:nil];
    
    NSString *finalResultText = [[[[self->result valueForKey:@"finalText"] componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
    NSString *normalizedDesiredResultText = [[[self->singleUtteranceTextEnglish componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];

    NSLog(@"final result: \n%@\ndesired:\n%@", finalResultText, normalizedDesiredResultText);
    XCTAssertTrue([finalResultText isEqualToString: normalizedDesiredResultText]);
}

@end

@interface SPXConversationTranscriberPullStreamTest : SPXConversationTranscriberTest {
    SPXPullAudioInputStream* stream;
}

- (void)setPullStreamAudio;

@end

@implementation SPXConversationTranscriberPullStreamTest

- (void) setPullStreamAudio {
    NSBundle *bundle = [NSBundle bundleForClass:[self class]];
    NSString *targetFile = [bundle pathForResource:singleUtteranceFileName ofType:@"wav"];
    NSURL *targetUrl = [NSURL URLWithString:targetFile];

    NSError *error = nil;
    AVAudioFile *audioFile = [[AVAudioFile alloc] initForReading:targetUrl commonFormat:AVAudioPCMFormatInt16 interleaved:NO error:&error];
    const NSInteger bytesPerFrame = audioFile.fileFormat.streamDescription->mBytesPerFrame;

    if (error)
    {
        NSLog(@"Error while opening file: %@", error);
        self->stream = nil;
        self.audioConfig = nil;
        return;
    }

    // check the format of the file
    NSAssert(1 == audioFile.fileFormat.channelCount, @"Bad channel count");
    NSAssert(16000 == audioFile.fileFormat.sampleRate, @"Unexpected sample rate");

    self->stream = [[SPXPullAudioInputStream alloc] initWithReadHandler: ^ NSInteger(NSMutableData *data, NSUInteger size) {
        
        AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFile.fileFormat frameCapacity:(AVAudioFrameCount) size / bytesPerFrame];
        NSError *bufferError = nil;
        bool success = [audioFile readIntoBuffer:buffer error:&bufferError];
        
        NSInteger nBytes = 0;
        if (!success) {
            // returns 0 to close the stream on read error.
            NSLog(@"Read error on stream: %@", bufferError);
        }
        else {
            // number of bytes in the buffer
            nBytes = [buffer frameLength] * bytesPerFrame;
            
            NSRange range;
            range.location = 0;
            range.length = nBytes;
            
            NSAssert(1 == buffer.stride, @"only one channel allowed");
            NSAssert(nil != buffer.int16ChannelData, @"assure correct format");
            
            [data replaceBytesInRange:range withBytes:buffer.int16ChannelData[0]];
            NSLog(@"%d bytes data returned", (int)[data length]);
        }
        // returns the number of bytes that have been read, 0 closes the stream.
        return nBytes;
    }
    getPropertyHandler: ^(SPXPropertyId propertyId) {
        if (propertyId == SPXDataBuffer_UserId) {
            NSLog(@"getPropertyHandler SPXDataBuffer_UserId");
            return @"speaker123";
        }
        else if (propertyId == SPXDataBuffer_TimeStamp)
        {
            NSLog(@"getPropertyHandler SPXDataBuffer_TimeStamp");
            return @"";
        }
        return @"";
    }
    closeHandler: ^(void) {
    }];
    
    self.audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:self->stream];
    self.conversationTranscriber = [[SPXConversationTranscriber alloc] initWithAudioConfiguration:self.audioConfig];
    [self setEventHandlers];
}

- (void) testPullStreamConversation {
    [self setSpeechConfig];
    [self setPullStreamAudio];
    [self testAddAndRemoveParticipantsFromConversation];
    NSString *finalResultText = [[[[self->result valueForKey:@"finalText"] componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
    NSString *normalizedDesiredResultText = [[[self->singleUtteranceTextEnglish componentsSeparatedByCharactersInSet:[[NSCharacterSet letterCharacterSet] invertedSet]] componentsJoinedByString:@""] lowercaseString];
    NSString *speakerId = [self->result valueForKey:@"userId"];
    NSLog(@"final result: \n%@\ndesired:\n%@\nuserId:\n%@", finalResultText, normalizedDesiredResultText, speakerId);
    XCTAssertTrue([finalResultText isEqualToString: normalizedDesiredResultText]);
    XCTAssertTrue([speakerId isEqualToString: @"speaker123"]);
}

@end
