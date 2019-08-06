//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <XCTest/XCTest.h>
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface SPXSpeechSynthesisEndtoEndTestBase : XCTestCase {
    double timeoutInSeconds;
    NSUInteger emptyWaveFileSize;
    NSUInteger guidLength;
    NSString *ssml;
}

- (void)setSpeechConfig;

- (void)checkResult:(SPXSpeechSynthesisResult *) result;

@property (nonatomic, assign) NSString * speechKey;
@property (nonatomic, assign) NSString * serviceRegion;
@property (nonatomic, retain) SPXSpeechConfiguration* speechConfig;
@property (nonatomic, retain) SPXAudioConfiguration* audioConfig;
@end

@implementation SPXSpeechSynthesisEndtoEndTestBase

- (void)setUp {
    [super setUp];
    timeoutInSeconds = 20.;
    emptyWaveFileSize = 46;
    guidLength = 32;
    ssml = @"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='en-GB'><voice name='Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)'>text</voice></speak>";

    self.speechKey = [[[NSProcessInfo processInfo] environment] objectForKey:@"subscriptionKey"];
    self.serviceRegion = [[[NSProcessInfo processInfo] environment] objectForKey:@"serviceRegion"];
    
    [self setSpeechConfig];
}

- (void)setSpeechConfig {
    
}

- (void)checkResult:(SPXSpeechSynthesisResult *)result {
    NSString *cancellationDetails;
    if ([result reason] == SPXResultReason_Canceled)
    {
        cancellationDetails = [[[SPXSpeechSynthesisCancellationDetails alloc] initFromCanceledSynthesisResult:result] errorDetails];
    }
    XCTAssertNotEqual([result reason], SPXResultReason_Canceled, @"The synthesis was canceled unexpectedly, with cancellation details=[%@].", cancellationDetails);
}

- (void)tearDown {
    [super tearDown];
}

- (void)_testDefaultSynthesis{
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]init:self.speechConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
}

- (void)_testPickLanguage{
    [self.speechConfig setSpeechSynthesisLanguage:@"en-GB"];
    NSString *lang = [self.speechConfig getSpeechSynthesisLanguage];
    XCTAssertEqualObjects(lang, @"en-GB");
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]init:self.speechConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
}

- (void)_testPickVoice{
    NSString *setVoiceName = @"Microsoft Server Speech Text to Speech Voice (en-GB, HazelRUS)";
    [self.speechConfig setSpeechSynthesisVoiceName:setVoiceName];
    NSString *getVoiceName = [self.speechConfig getSpeechSynthesisVoiceName];
    XCTAssertEqualObjects(setVoiceName, getVoiceName);
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]init:self.speechConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
}

- (void)_testSynthesisOutputToFile{
    NSString* fileName = @"wavefile.wav";
    SPXAudioConfiguration* fileConfig = [[SPXAudioConfiguration alloc] initWithWavFileInput:fileName];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:fileConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    [self checkResult:result1];
    NSUInteger fileSize = [[[NSFileManager defaultManager] attributesOfItemAtPath:fileName error:nil] fileSize];
    XCTAssertGreaterThan(fileSize, emptyWaveFileSize, @"The size of output wave file 1 is unexpected. Expected: greater than %lu, Actual: %lu", emptyWaveFileSize, fileSize);
    
    result1 = nil;
    synthesizer = nil; // wavefile.wav is now closed

    // Make a second run with 2 speaks to verify that the audio can be append to the file while speaking
    synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:fileConfig];
    result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakSsml:ssml];
    [self checkResult:result1];
    [self checkResult:result2];
    NSUInteger fileSize2 = [[[NSFileManager defaultManager] attributesOfItemAtPath:fileName error:nil] fileSize];
    XCTAssertGreaterThan(fileSize2, fileSize, @"The size of output wave file 1 and wave file 2 are unexpected. Expected: wave size 2 > wave size 1, Actual: wave size 2 = %lu, wave size 1 = %lu", fileSize2, fileSize);
}

- (void)_testSynthesisOutputToPushStream {
    __block BOOL isClosed = false;
    SPXPushAudioOutputStream* stream = [[SPXPushAudioOutputStream alloc]
                                        initWithWriteHandler:
                                        ^NSUInteger(NSData *data) {
                                            XCTAssertGreaterThan([data length], 0, @"The size of data chunck passed to write handler is empty.");
                                            return [data length];
                                        }
                                        closeHandler:
                                        ^(void) {
                                            isClosed = true;
                                        }];
    SPXAudioConfiguration* streamConfig = [[SPXAudioConfiguration alloc] initWithStreamOutput:stream];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:streamConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakSsml:ssml];
    [self checkResult:result1];
    [self checkResult:result2];
    result1 = nil;
    result2 = nil;
    synthesizer = nil;
    XCTAssertTrue(isClosed, @"The push audio output stream is not closed.");
}

- (void)_testSynthesisOutputToPullStreamUseAfterSynthesisCompleted {
    SPXPullAudioOutputStream* stream = [[SPXPullAudioOutputStream alloc] init];
    SPXAudioConfiguration* streamConfig = [[SPXAudioConfiguration alloc] initWithStreamOutput:stream];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:streamConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
    result1 = nil;
    result2 = nil;
    synthesizer = nil;
    NSMutableData* data = [[NSMutableData alloc] initWithCapacity:1024];
    NSUInteger audioLength = 0;
    while ([stream read:data length:1024] > 0) {
        audioLength += [data length];
    }
    XCTAssertGreaterThan(audioLength, 0);
}

- (void)_testSynthesisOutputToPullStreamStartUsingBeforeDoneSynthesizing {
    SPXPullAudioOutputStream* stream = [[SPXPullAudioOutputStream alloc] init];

    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSMutableData* data = [[NSMutableData alloc] initWithCapacity:1024];
        NSUInteger audioLength = 0;
        while ([stream read:data length:1024] > 0) {
            audioLength += [data length];
        }
        XCTAssertGreaterThan(audioLength, 0);
        dispatch_semaphore_signal(semaphore);
    });

    SPXAudioConfiguration* streamConfig = [[SPXAudioConfiguration alloc] initWithStreamOutput:stream];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:streamConfig];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
    result1 = nil;
    result2 = nil;
    synthesizer = nil;
    dispatch_semaphore_wait(semaphore,DISPATCH_TIME_FOREVER);
}


- (void)_testSpeakOutInResults {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    [self checkResult:result1];
    XCTAssertEqualWithAccuracy(guidLength, result1.resultId.length, 0.01, @"The length of result ID should be the length of a GUID (32).");
    XCTAssertEqual(SPXResultReason_SynthesizingAudioCompleted, result1.reason, @"The synthesis should be completed now.");
    NSUInteger audioLength = result1.audioData.length;
    XCTAssertGreaterThan(audioLength, emptyWaveFileSize, @"The audio data size should be greater than %lu, but actually it's %lu.", emptyWaveFileSize, audioLength);
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result2];
    XCTAssertEqualWithAccuracy(guidLength, result2.resultId.length, 0.01, @"The length of result ID should be the length of a GUID (32).");
    XCTAssertEqual(SPXResultReason_SynthesizingAudioCompleted, result2.reason, @"The synthesis should be completed now.");
    audioLength = result2.audioData.length;
    XCTAssertGreaterThan(audioLength, emptyWaveFileSize, @"The audio data size should be greater than %lu, but actually it's %lu.", emptyWaveFileSize, audioLength);
}

- (void)_testSpeakOutputInChunksInEventSynthesizing {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    [synthesizer addSynthesizingEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs){
        XCTAssertEqual(SPXResultReason_SynthesizingAudio, eventArgs.result.reason, @"The synthesis should be on going now.");
        XCTAssertGreaterThan(eventArgs.result.audioData.length, 0, @"The audio chunk size should be greater than zero");
    }];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result1];
    [self checkResult:result2];
}

- (void)_testSpeakOutputInStreams {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    [self checkResult:result1];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioCompleted, result1.reason, @"The synthesis should be completed now.");
    NSUInteger audioLength = result1.audioData.length;
    XCTAssertGreaterThan(audioLength, emptyWaveFileSize, @"The audio data size should be greater than %lu, but actually it's %lu.", emptyWaveFileSize, audioLength);
    
    SPXAudioDataStream* stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result1];
    NSMutableData* data = [[NSMutableData alloc]initWithCapacity:1024];
    NSUInteger readSize;
    NSUInteger audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertEqual(audioLength - emptyWaveFileSize, audioLengthInDataStream); // audio data in SPXSpeechSynthesisResult has a header while data in stream doesn't

    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakText:@"{{{text2}}}"];
    [self checkResult:result2];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioCompleted, result2.reason, @"The synthesis should be completed now.");
    audioLength = result2.audioData.length;
    XCTAssertGreaterThan(audioLength, emptyWaveFileSize, @"The audio data size should be greater than %lu, but actually it's %lu.", emptyWaveFileSize, audioLength);

    stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result2];
    audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertEqual(audioLength - emptyWaveFileSize, audioLengthInDataStream);
}

- (void)_testSpeakOutputInStreamsBeforeDoneFromEventSynthesisStarted {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    __block SPXAudioDataStream* stream;
    __block dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [synthesizer addSynthesisStartedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
        XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, eventArgs.result.reason, @"The synthesis should be started now.");
        XCTAssertEqual(0, eventArgs.result.audioData.length, @"The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
        stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:eventArgs.result];
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            NSMutableData* data = [[NSMutableData alloc] initWithCapacity:1024];
            NSUInteger audioLength = 0;
            while ([stream readData:data length:1024] > 0) {
                audioLength += [data length];
            }
            XCTAssertGreaterThan(audioLength, 0);
            dispatch_semaphore_signal(semaphore);
        });
    }];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:@"{{{text1}}}"];
    [self checkResult:result1];
    result1 = nil;
    synthesizer = nil;
    dispatch_semaphore_wait(semaphore,DISPATCH_TIME_FOREVER);
}

- (void)_testSpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingText {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    SPXSpeechSynthesisResult* result1 = [synthesizer StartSpeakingText:@"{{{text1}}}"];
    [self checkResult:result1];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, result1.reason, @"The synthesis should be started now.");
    XCTAssertEqual(0, result1.audioData.length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
    SPXSpeechSynthesisResult* result2 = [synthesizer StartSpeakingText:@"{{{text2}}}"];
    [self checkResult:result2];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, result2.reason, @"The synthesis should be started now.");
    XCTAssertEqual(0, result2.audioData.length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
    
    SPXAudioDataStream* stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result1];
    NSMutableData* data = [[NSMutableData alloc]initWithCapacity:1024];
    NSUInteger readSize;
    NSUInteger audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertGreaterThan(audioLengthInDataStream, emptyWaveFileSize);

    stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result2];
    audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertGreaterThan(audioLengthInDataStream, emptyWaveFileSize);
}

- (void)_testSpeakOutputInStreamsBeforeDoneQueued {
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    __block NSUInteger startedRequests = 0;
    [synthesizer addSynthesisStartedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
        XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, eventArgs.result.reason, @"The synthesis should be started now.");
        XCTAssertEqual(0, eventArgs.result.audioData.length, @"The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
        startedRequests++;
    }];

    __block NSUInteger completedRequests = 0;
    __block NSUInteger startedRequestsWhenFirstRequestWasCompleted = 0;
    [synthesizer addSynthesisCompletedEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
        XCTAssertEqual(SPXResultReason_SynthesizingAudioCompleted, eventArgs.result.reason);
        XCTAssertGreaterThan(eventArgs.result.audioData.length, self->emptyWaveFileSize, @"The audio data size should be greater than %lu, but actually it's %lu.", self->emptyWaveFileSize, eventArgs.result.audioData.length);
        completedRequests++;
        if (1 == completedRequests) {
            startedRequestsWhenFirstRequestWasCompleted = startedRequests;
        }
    }];
    
    __block NSUInteger canceledRequests = 0;
    [synthesizer addSynthesisCanceledEventHandler:^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisEventArgs *eventArgs) {
        canceledRequests++;
    }];

    SPXSpeechSynthesisResult* result1 = [synthesizer StartSpeakingText:@"{{{text1}}}"];
    [self checkResult:result1];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, result1.reason, @"The synthesis should be started now.");
    XCTAssertEqual(0, result1.audioData.length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
    SPXSpeechSynthesisResult* result2 = [synthesizer StartSpeakingSsml:ssml];
    [self checkResult:result2];
    XCTAssertEqual(SPXResultReason_SynthesizingAudioStarted, result2.reason, @"The synthesis should be started now.");
    XCTAssertEqual(0, result2.audioData.length, "The synthesized audio should still be zero at this point, since the synthesis just started, and no audio chunk has been synthesized yet.");
    
    SPXAudioDataStream* stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result1];
    NSMutableData* data = [[NSMutableData alloc]initWithCapacity:1024];
    NSUInteger readSize;
    NSUInteger audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertGreaterThan(audioLengthInDataStream, emptyWaveFileSize);

    stream = [[SPXAudioDataStream alloc] initFromSynthesisResult:result2];
    audioLengthInDataStream = 0;
    while (true) {
        readSize = [stream readData:data length:1024];
        if (readSize == 0)
            break;
        audioLengthInDataStream += readSize;
        XCTAssertEqual(readSize, data.length, @"The returned length and buffer length should be equal.");
    }
    XCTAssertGreaterThan(audioLengthInDataStream, emptyWaveFileSize);
    XCTAssertEqual(1, startedRequestsWhenFirstRequestWasCompleted);
    XCTAssertEqual(0, canceledRequests, @"There should be no canceled requests.");
}

- (void)_testCheckWordBoundaryEvents {
    [self.speechConfig setSpeechSynthesisVoiceName:@"Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)"];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:self.speechConfig audioConfiguration:nil];
    NSString* plainText = @"您好，我是来自Microsoft的中文声音。";
    NSString* ssml = @"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xmlns:emo='http://www.w3.org/2009/10/emotionml' xml:lang='zh-CN'><voice name='Microsoft Server Speech Text to Speech Voice (zh-CN, HuihuiRUS)'>您好，<break time='50ms'/>我是来自Microsoft的中文声音。</voice></speak>";
    NSUInteger expectedAudioOffsets[8] = { 500000, 7443750, 9175625, 10585000, 15096875, 22924375, 25213125, 28760000 };
    NSUInteger expectedTextOffsets[8] = { 0, 3, 4, 5, 7, 16, 17, 19 };
    NSUInteger expectedWordLengths[8] = { 2, 1, 1, 2, 9, 1, 2, 2 };
    NSUInteger* expectedAudioOffsetsPtr = expectedAudioOffsets;
    NSUInteger* expectedTextOffsetsPtr = expectedTextOffsets;
    NSUInteger* expectedWordLengthsPtr = expectedWordLengths;
    __block NSUInteger currentIndex = 0;

    [synthesizer addSynthesisWordBoundaryEventHandler: ^ (SPXSpeechSynthesizer *synthesizer, SPXSpeechSynthesisWordBoundaryEventArgs *eventArgs) {
        XCTAssertLessThan(currentIndex, 8, @"Incorrect count of word boundary events.");
        XCTAssertEqualWithAccuracy(expectedAudioOffsetsPtr[currentIndex], eventArgs.audioOffset, 0.1, @"Audio offset mismatch on word #%lu.", currentIndex + 1);
        XCTAssertEqualWithAccuracy(expectedTextOffsetsPtr[currentIndex], eventArgs.textOffset, 0.1, @"Text offset mismatch on word #%lu.", currentIndex + 1);
        XCTAssertEqualWithAccuracy(expectedWordLengthsPtr[currentIndex], eventArgs.wordLength, 0.1, @"Word length mismatch on word #%lu.", currentIndex + 1);
        currentIndex++;
    }];
    SPXSpeechSynthesisResult* result1 = [synthesizer SpeakText:plainText];
    [self checkResult:result1];
    XCTAssertEqual(8, currentIndex, @"Incorrect count of word boundary events.");
    for (int i = 1; i < 8; i++) {
        expectedAudioOffsets[i] += 500000;
        expectedTextOffsets[i] += 271; // basic offset of ssml
    }
    expectedAudioOffsets[5] = 23211875;
    expectedAudioOffsets[6] = 25500625;
    expectedAudioOffsets[7] = 29029375;
    expectedTextOffsets[0] = 251;
    currentIndex = 0;
    SPXSpeechSynthesisResult* result2 = [synthesizer SpeakSsml:ssml];
    [self checkResult:result2];
    XCTAssertEqual(8, currentIndex, @"Incorrect count of word boundary events.");
}

- (void)_testAuthorizationToken {
    SPXSpeechConfiguration* speechConfigWithToken = [[SPXSpeechConfiguration alloc] initWithAuthorizationToken:@"InvalidToken1" region:self.serviceRegion];
    SPXSpeechSynthesizer* synthesizer = [[SPXSpeechSynthesizer alloc]initWithSpeechConfiguration:speechConfigWithToken audioConfiguration:nil];
    XCTAssertEqualObjects(@"InvalidToken1", synthesizer.authorizationToken);

    synthesizer.authorizationToken = @"InvalidToken2";
    XCTAssertEqualObjects(@"InvalidToken2", synthesizer.authorizationToken);
}

@end

@interface SPXSpeechSynthesisEndtoEndTest : SPXSpeechSynthesisEndtoEndTestBase {
}

- (void)setSpeechConfig;

@end

@implementation SPXSpeechSynthesisEndtoEndTest

- (void)setSpeechConfig {
    self.speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
}

- (void)testDefaultSynthesis {
    return [self _testDefaultSynthesis];
}

- (void)testPickLanguage {
    return [self _testPickLanguage];
}

- (void)testPickVoice {
    return [self _testPickVoice];
}

- (void)testSynthesisOutputToFile {
    return [self _testSynthesisOutputToFile];
}

- (void)testSynthesisOutputToPushStream {
    return [self _testSynthesisOutputToPushStream];
}

- (void)testSynthesisOutputToPullStreamUseAfterSynthesisCompleted {
    return [self _testSynthesisOutputToPullStreamUseAfterSynthesisCompleted];
}

- (void)testSynthesisOutputToPullStreamStartUsingBeforeDoneSynthesizing {
    return [self _testSynthesisOutputToPullStreamStartUsingBeforeDoneSynthesizing];
}

- (void)testSpeakOutInResults {
    return [self _testSpeakOutInResults];
}

- (void)testSpeakOutputInChunksInEventSynthesizing {
    return [self _testSpeakOutputInChunksInEventSynthesizing];
}

- (void)testSpeakOutputInStreams {
    return [self _testSpeakOutputInStreams];
}

- (void)testSpeakOutputInStreamsBeforeDoneFromEventSynthesisStarted {
    return [self _testSpeakOutputInStreamsBeforeDoneFromEventSynthesisStarted];
}

- (void)testSpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingText {
    return [self _testSpeakOutputInStreamsBeforeDoneFromMethodStartSpeakingText];
}

- (void)testSpeakOutputInStreamsBeforeDoneQueued {
    return [self _testSpeakOutputInStreamsBeforeDoneQueued];
}

- (void)testAuthorizationToken {
    return [self _testAuthorizationToken];
}

@end

@interface SPXSpeechSynthesisEndtoEndTestUsp : SPXSpeechSynthesisEndtoEndTestBase {
}

- (void)setSpeechConfig;

@end

@implementation SPXSpeechSynthesisEndtoEndTestUsp

- (void)setSpeechConfig {
    self.speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:self.speechKey region:self.serviceRegion];
    [self.speechConfig setPropertyTo:@"true" byName:@"CARBON-INTERNAL-UseTtsEngine-Usp"];
}

- (void)testDefaultSynthesis {
    return [self _testDefaultSynthesis];
}

- (void)testSynthesisOutputToFile {
    return [self _testSynthesisOutputToFile];
}

- (void)testSynthesisOutputToPushStream {
    return [self _testSynthesisOutputToPushStream];
}

- (void)testSpeakOutInResults {
    return [self _testSpeakOutInResults];
}

- (void)testCheckWordBoundaryEvents {
    return [self _testCheckWordBoundaryEvents];
}

@end
