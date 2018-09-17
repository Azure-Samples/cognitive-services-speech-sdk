//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speech_enums_h
#define speech_enums_h

typedef NS_ENUM(NSUInteger, OutputFormat)
{
    Simple = 0,
    Detailed = 1
};

/**
  * Specifies the possible reasons a recognition result might be generated.
  */
typedef NS_ENUM(NSUInteger, ResultReason)
{
    /**
      * Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
      */
    NoMatch = 0,
    
    /**
      * Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
      */
    Canceled = 1,
    
    /**
      * Indicates the speech result contains hypothesis text.
      */
    RecognizingSpeech = 2,
    
    /**
      * Indicates the speech result contains final text that has been recognized.
      * Speech Recognition is now complete for this phrase.
      */
    RecognizedSpeech = 3,
    
    /**
      * Indicates the intent result contains hypothesis text and intent.
      */
    RecognizingIntent = 4,
    
    /**
      * Indicates the intent result contains final text and intent.
      * Speech Recognition and Intent determination are now complete for this phrase.
      */
    RecognizedIntent = 5,
    
    /**
      * Indicates the translation result contains hypothesis text and its translation(s).
      */
    TranslatingSpeech = 6,
    
    /**
      * Indicates the translation result contains final text and corresponding translation(s).
      * Speech Recognition and Translation are now complete for this phrase.
      */
    TranslatedSpeech = 7,
    
    /**
      * Indicates the synthesized audio result contains a non-zero amount of audio data
      */
    SynthesizingAudio = 8,
    
    /**
      * Indicates the synthesized audio is now complete for this phrase.
      */
    SynthesizingAudioComplete = 9
};

/**
  * Defines the possible reasons a recognition result might be canceled.
  */
typedef NS_ENUM(NSUInteger, CancellationReason)
{
    /**
      * Indicates that an error occurred during speech recognition. The ErrorDetails property contains detailed error response.
      */
    Error = 1,
    
    /**
      * Indicates that the end of the audio stream was reached.
      */
    EndOfStream = 2,
};

/**
  * Defines the possible reasons a recognition result might not be recognized.
  */
typedef NS_ENUM(NSUInteger, NoMatchReason)
{
    /**
      * Indicates that speech was detected, but not recognized.
      */
    NotRecognized = 1,
    
    /**
      * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
      */
    InitialSilenceTimeout = 2,
    
    /**
      * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
      */
    InitialBabbleTimeout = 3
};

#endif /* speech_enums_h */
