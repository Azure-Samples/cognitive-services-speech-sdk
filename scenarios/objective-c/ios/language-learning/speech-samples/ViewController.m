//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import "ViewController.h"
#import "AudioRecorder.h"
#import <AVFoundation/AVFoundation.h>
#import <MicrosoftCognitiveServicesSpeech/SPXSpeechApi.h>

@interface ViewController () {
    NSString *speechKey;
    NSString *serviceRegion;
    NSString *pronunciationAssessmentReferenceText;
    AudioRecorder *recorder;
}

@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromMicButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromFileButton;
@property (strong, nonatomic) IBOutlet UIButton *pronunciationAssessFromStreamButton;

@property (strong, nonatomic) IBOutlet UILabel *recognitionResultLabel;

- (IBAction)pronunciationAssessFromMicButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessFromFileButtonTapped:(UIButton *)sender;
- (IBAction)pronunciationAssessFromStreamButtonTapped:(UIButton *)sender;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    speechKey = @"YourSubscriptionKey";
    serviceRegion = @"YourServiceRegion";
    pronunciationAssessmentReferenceText = @"Hello world.";

    [self.view setBackgroundColor:[UIColor whiteColor]];

    self.pronunciationAssessFromMicButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromMicButton addTarget:self action:@selector(pronunciationAssessFromMicButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromMicButton setTitle:[NSString stringWithFormat:@"Start pronunciation assessment \n (Read out \"%@\")", pronunciationAssessmentReferenceText] forState:UIControlStateNormal];
    [self.pronunciationAssessFromMicButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromMicButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromMicButton setFrame:CGRectMake(50.0, 110.0, 300.0, 40.0)];
    self.pronunciationAssessFromMicButton.accessibilityIdentifier = @"pronunciation_assessment_button";
    [self.view addSubview:self.pronunciationAssessFromMicButton];

    self.pronunciationAssessFromFileButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromFileButton addTarget:self action:@selector(pronunciationAssessFromFileButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromFileButton setTitle:@"Start continuous pronunciation assessment from file." forState:UIControlStateNormal];
    [self.pronunciationAssessFromFileButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromFileButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromFileButton setFrame:CGRectMake(50.0, 170.0, 300.0, 40.0)];
    self.pronunciationAssessFromFileButton.accessibilityIdentifier = @"pronunciation_assessment_file_button";
    [self.view addSubview:self.pronunciationAssessFromFileButton];

    self.pronunciationAssessFromStreamButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.pronunciationAssessFromStreamButton addTarget:self action:@selector(pronunciationAssessFromStreamButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.pronunciationAssessFromStreamButton setTitle:@"Start pronunciation assessment from stream." forState:UIControlStateNormal];
    [self.pronunciationAssessFromStreamButton titleLabel].lineBreakMode = NSLineBreakByWordWrapping;
    [self.pronunciationAssessFromStreamButton titleLabel].textAlignment = NSTextAlignmentCenter;
    [self.pronunciationAssessFromStreamButton setFrame:CGRectMake(50.0, 230.0, 300.0, 40.0)];
    self.pronunciationAssessFromStreamButton.accessibilityIdentifier = @"pronunciation_assessment_stream_button";
    [self.view addSubview:self.pronunciationAssessFromStreamButton];

    self.recognitionResultLabel = [[UILabel alloc] initWithFrame:CGRectMake(50.0, 330.0, 300.0, 300.0)];
    self.recognitionResultLabel.lineBreakMode = NSLineBreakByWordWrapping;
    self.recognitionResultLabel.numberOfLines = 0;
    self.recognitionResultLabel.accessibilityIdentifier = @"result_label";
    [self.recognitionResultLabel setText:@"Press a button!"];

    [self.view addSubview:self.recognitionResultLabel];
}

- (IBAction)pronunciationAssessFromMicButtonTapped:(UIButton *)sender {
    if ([[(UIButton *)sender currentTitle]isEqualToString:@"Stop recording"])
    {
        [self->recorder stop];
        [self.pronunciationAssessFromMicButton setTitle:[NSString stringWithFormat:@"Start pronunciation assessment \n (Read out \"%@\")", self->pronunciationAssessmentReferenceText] forState:UIControlStateNormal];
    }
    else
    {
        [self.pronunciationAssessFromMicButton setTitle:@"Stop recording" forState:UIControlStateNormal];
        dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
            [self pronunciationAssessFromMicrophone];
        });
    }
}

- (IBAction)pronunciationAssessFromFileButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self pronunciationAssessFromFile];
    });
}

- (IBAction)pronunciationAssessFromStreamButtonTapped:(UIButton *)sender {
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_DEFAULT, 0), ^{
        [self pronunciationAssessFromStream];
    });
}

/*
 * Performs single-shot pronunciation assessment from microphone.
 */
- (void)pronunciationAssessFromMicrophone {
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXPushAudioInputStream *stream = [[SPXPushAudioInputStream alloc] init];
    self->recorder = [[AudioRecorder alloc]initWithPushStream:stream];
    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc]initWithStreamInput:stream];

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:pronunciationAssessmentReferenceText
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:true];
    
    [pronunicationConfig enableProsodyAssessment];
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];
    [self->recorder record];

    SPXSpeechRecognitionResult *speechResult = [speechRecognizer recognizeOnce];
    if (SPXResultReason_Canceled == speechResult.reason) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:speechResult];
        NSLog(@"Speech recognition was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
    } else if (SPXResultReason_RecognizedSpeech == speechResult.reason) {
        NSLog(@"Speech recognition result received: %@", speechResult.text);
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:speechResult];
        NSString *resultText = [NSString stringWithFormat:@"Assessment finished. \nAccuracy score: %f, Prosody score: %f, Pronunciation score: %f, Completeness Score: %f, Fluency score: %f.", pronunciationResult.accuracyScore, pronunciationResult.prosodyScore,  pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        [self updateRecognitionResultText:resultText];
    } else {
        NSLog(@"There was an error.");
        [self updateRecognitionErrorText:(@"Speech Recognition Error")];
    }

    [self->recorder stop];
}

/*
 * Performs continuous pronunciation assessment from file.
 */
- (void)pronunciationAssessFromFile {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSString *pronFile = [mainBundle pathForResource: @"pronunciation-assessment" ofType:@"wav"];
    NSLog(@"pronFile path: %@", pronFile);
    if (!pronFile) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:(@"Cannot find audio file")];
        return;
    }

    SPXAudioConfiguration* pronAudioSource = [[SPXAudioConfiguration alloc] initWithWavFileInput:pronFile];
    if (!pronAudioSource) {
        NSLog(@"Loading audio file failed!");
        [self updateRecognitionErrorText:(@"Audio Error")];
        return;
    }

    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    if (!speechConfig) {
        NSLog(@"Could not load speech config");
        [self updateRecognitionErrorText:(@"Speech Config Error")];
        return;
    }

    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:pronAudioSource];
    if (!speechRecognizer) {
        NSLog(@"Could not create speech recognizer");
        [self updateRecognitionResultText:(@"Speech Recognition Error")];
        return;
    }

    // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    // The audio text is "Hello hello world! Today is a day!"
    SPXPronunciationAssessmentConfiguration *pronunicationConfig =
    [[SPXPronunciationAssessmentConfiguration alloc] init:@"Hello world! Today is a nice day!"
                                            gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark
                                              granularity:SPXPronunciationAssessmentGranularity_Phoneme
                                             enableMiscue:true];
    
    [pronunicationConfig enableProsodyAssessment];
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
    [pronunicationConfig applyToRecognizer:speechRecognizer];
    [self updateRecognitionStatusText:(@"Assessing...")];

    // connect callbacks
    __block double sumProsody = 0;
    __block int sumWords = 0;
    __block int countProsody = 0;
    NSMutableArray *recognizedWords = [NSMutableArray array];
    
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld. offset %llu duration %llu resultid:%@", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason, eventArgs.result.offset, eventArgs.result.duration, eventArgs.result.resultId);
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc]init:eventArgs.result];
        NSString *resultText = [NSString stringWithFormat:@"Received final result event. \nrecognition result: %@, Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f.", eventArgs.result.text, pronunciationResult.accuracyScore, pronunciationResult.prosodyScore, pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        sumProsody += pronunciationResult.prosodyScore;
        countProsody += 1;
        [self updateRecognitionResultText:resultText];
        NSArray *words = [eventArgs.result.text componentsSeparatedByString:@" "];
        NSUInteger wordCount = [words count];
        sumWords += wordCount;
        
        [recognizedWords addObjectsFromArray:pronunciationResult.words];
    }];

    __block bool end = false;
    [speechRecognizer addCanceledEventHandler:^(SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Pronunciation assessment was canceled: %@. Did you pass the correct key/region combination?", details.errorDetails);
        [self updateRecognitionErrorText:([NSString stringWithFormat:@"Canceled: %@", details.errorDetails ])];
        end = true;
    }];

    // session stopped callback to recognize stream has ended
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received session stopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];

    // start recognizing
    [speechRecognizer startContinuousRecognition];

    // wait until a session stopped event has been received
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];

    if (sumWords > 0) {
        // Accuracy score
        double totalAccurayScore = 0;
        int accuracyCount = 0;
        int validCount = 0;
        double durationSum = 0.0;
        
        for (SPXWordLevelTimingResult *word in recognizedWords) {
            if (![word.errorType isEqualToString:@"Insertion"]) {
                totalAccurayScore += word.accuracyScore;
                accuracyCount += 1;
            }
            
            if ([word.errorType isEqualToString:@"None"]) {
                durationSum += word.duration + 0.01;
                validCount += 1;
            }
            
        }
        double accurayScore = (accuracyCount > 0) ? (totalAccurayScore) / accuracyCount : NAN;
        
        // Fluency score
        SPXWordLevelTimingResult *firstWord = [recognizedWords firstObject];
        double startOffset = firstWord.offset;

        SPXWordLevelTimingResult *lastWord = [recognizedWords lastObject];
        double endOffset = lastWord.offset + lastWord.duration + 0.01;

        double fluencyScore = durationSum / (endOffset - startOffset) * 100.0;
        
        // Completeness score
        double completenessScore = (double)validCount / (double)accuracyCount * 100.0;
        if (completenessScore > 100) {
            completenessScore = 100;
        }
        
        // Prosody score
        double prosodyScore = sumProsody / countProsody;
        
        double minScore = MIN(accurayScore, MIN(prosodyScore, MIN(completenessScore, fluencyScore)));
        
        // Pronunciation score
        double pronunciationScore = 0.2 * (accurayScore + prosodyScore + completenessScore + fluencyScore) + 0.2 * minScore;
        
        // Overall scores.
        NSString *resultText = [NSString stringWithFormat:@"Assessment finished. \nOverall accuracy score: %.2f, prosody score: %.2f, fluency score: %.2f, completeness score: %.2f, pronunciation score: %.2f", accurayScore, prosodyScore, fluencyScore, completenessScore, pronunciationScore];
        [self updateRecognitionResultText:resultText];
        
        for (NSInteger idx = 0; idx < recognizedWords.count; idx++) {
            SPXWordLevelTimingResult *word = recognizedWords[idx];
            NSLog(@"    %ld: word: %@\taccuracy score: %.2f\terror type: %@",
                  (long)(idx + 1), word.word, word.accuracyScore, word.errorType);
        }
    }
}

/*
 * Performs pronunciation assessment from stream.
 */
- (void)pronunciationAssessFromStream {
    [self updateRecognitionResultText:@"Stream Result"];
    // Creates an instance of a speech config with specified subscription key and service region.
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *path = [bundle pathForResource:@"whatstheweatherlike" ofType:@"wav"];
    if (!path) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:@"Cannot find audio file"];
        return;
    }
    NSLog(@"pronunciation assessment audio file path: %@", path);

    NSData *audioDataWithHeader = [NSData dataWithContentsOfFile:path];
    NSData *audioData = [audioDataWithHeader subdataWithRange:NSMakeRange(46, audioDataWithHeader.length - 46)];

    NSDate *startTime = [NSDate date];

    SPXAudioStreamFormat *audioFormat = [[SPXAudioStreamFormat alloc] initUsingPCMWithSampleRate:16000 bitsPerSample:16 channels:1];
    SPXPushAudioInputStream *audioInputStream = [[SPXPushAudioInputStream alloc] initWithAudioFormat:audioFormat];
    if (!audioInputStream) {
        NSLog(@"Error: Failed to create audio input stream.");
        return;
    }

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc] initWithStreamInput:audioInputStream];
    if (!audioConfig) {
        NSLog(@"Error: audioConfig is Nil");
        return;
    }

    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];

    NSString *referenceText = @"what's the weather like";
    SPXPronunciationAssessmentConfiguration *pronAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc] init:referenceText gradingSystem:SPXPronunciationAssessmentGradingSystem_HundredMark granularity:SPXPronunciationAssessmentGranularity_Phoneme enableMiscue:true];
    
    [pronAssessmentConfig enableProsodyAssessment];
    
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
    [pronAssessmentConfig applyToRecognizer:speechRecognizer error:nil];

    [audioInputStream write:audioData];
    [audioInputStream write:[NSData data]];

    [self updateRecognitionResultText:@"Analysising"];

    // Handle the recognition result
    [speechRecognizer recognizeOnceAsync:^(SPXSpeechRecognitionResult * _Nullable result) {
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc] init:result];
        if (!pronunciationResult) {
            NSLog(@"Error: pronunciationResult is Nil");
            return;
        }
        
        [self updateRecognitionResultText:@"generating result..."];
        NSMutableString *mResult = [[NSMutableString alloc] init];
        NSString *resultText = [NSString stringWithFormat:@"Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f", pronunciationResult.accuracyScore, pronunciationResult.pronunciationScore, pronunciationResult.prosodyScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        NSLog(@"%@", resultText);
        [mResult appendString:resultText];
        [mResult appendString:@"\n\nword  accuracyScore   errorType\n"];
        for(SPXWordLevelTimingResult *word in pronunciationResult.words){
            NSString *wordLevel = [NSString stringWithFormat:@"%@   %.2f  %@\n", word.word, word.accuracyScore, word.errorType];
            [mResult appendString:wordLevel];
            NSLog(@"%@", wordLevel);
        }
        NSString *finalResult = [NSString stringWithString:mResult];
        NSLog(@"%@", finalResult);
        [self updateRecognitionResultText:finalResult];
        
        NSDate *endTime = [NSDate date];
        double timeCost = [endTime timeIntervalSinceDate:startTime] * 1000;
        NSLog(@"Time cost: %fms", timeCost);
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

/*
 * Performs pronunciation assessment configured with json
 */
- (void)pronunciationAssessConfiguredWithJson {
    [self updateRecognitionResultText:@"Result configured with json"];
    // Creates an instance of a speech config with specified subscription key and service region.
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:serviceRegion];
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *path = [bundle pathForResource:@"whatstheweatherlike" ofType:@"wav"];
    if (!path) {
        NSLog(@"Cannot find audio file!");
        [self updateRecognitionErrorText:@"Cannot find audio file"];
        return;
    }
    NSLog(@"pronunciation assessment audio file path: %@", path);

    SPXAudioConfiguration *audioConfig = [[SPXAudioConfiguration alloc]initWithWavFileInput:path];

    SPXSpeechRecognizer *speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig language:@"en-US" audioConfiguration:audioConfig];

    NSString *referenceText = @"what's the weather like";
    // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
    NSString *jsonConfig = @"{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true,\"ScenarioId\":\"[scenario ID will be assigned by product team]\"}";
    SPXPronunciationAssessmentConfiguration *pronAssessmentConfig = [[SPXPronunciationAssessmentConfiguration alloc] initWithJson:jsonConfig error:nil];
    pronAssessmentConfig.referenceText = referenceText;
    
    [pronAssessmentConfig enableProsodyAssessment];
    
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *sender, SPXSessionEventArgs *eventArgs) {
        NSLog(@"SESSION ID: %@", eventArgs.sessionId);
    }];
    
    [pronAssessmentConfig applyToRecognizer:speechRecognizer error:nil];

    [self updateRecognitionResultText:@"Analysising"];

    // Handle the recognition result
    [speechRecognizer recognizeOnceAsync:^(SPXSpeechRecognitionResult * _Nullable result) {
        SPXPronunciationAssessmentResult *pronunciationResult = [[SPXPronunciationAssessmentResult alloc] init:result];
        if (!pronunciationResult) {
            NSLog(@"Error: pronunciationResult is Nil");
            return;
        }
        
        [self updateRecognitionResultText:@"generating result..."];
        NSMutableString *mResult = [[NSMutableString alloc] init];
        NSString *resultText = [NSString stringWithFormat:@"Accuracy score: %.2f, Prosody score: %.2f, Pronunciation score: %.2f, Completeness Score: %.2f, Fluency score: %.2f", pronunciationResult.accuracyScore,  pronunciationResult.prosodyScore, pronunciationResult.pronunciationScore, pronunciationResult.completenessScore, pronunciationResult.fluencyScore];
        NSLog(@"%@", resultText);
        [mResult appendString:resultText];
        [mResult appendString:@"\n\nword  accuracyScore   errorType\n"];
        for(SPXWordLevelTimingResult *word in pronunciationResult.words){
            NSString *wordLevel = [NSString stringWithFormat:@"%@   %.2f  %@\n", word.word, word.accuracyScore, word.errorType];
            [mResult appendString:wordLevel];
            NSLog(@"%@", wordLevel);
        }
        NSString *finalResult = [NSString stringWithString:mResult];
        NSLog(@"%@", finalResult);
        [self updateRecognitionResultText:finalResult];
        dispatch_semaphore_signal(semaphore);
    }];
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
}

- (void)updateRecognitionResultText:(NSString *) resultText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.blackColor;
        self.recognitionResultLabel.text = resultText;
    });
}

- (void)updateRecognitionErrorText:(NSString *) errorText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.redColor;
        self.recognitionResultLabel.text = errorText;
    });
}

- (void)updateRecognitionStatusText:(NSString *) statusText {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.recognitionResultLabel.textColor = UIColor.grayColor;
        self.recognitionResultLabel.text = statusText;
    });
}

@end

