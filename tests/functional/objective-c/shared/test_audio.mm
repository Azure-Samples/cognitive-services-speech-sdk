//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>

#import "test_audio.h"
#include <iostream>
#include <fstream>

constexpr uint16_t tagBufferSize = 4;
constexpr uint16_t chunkTypeBufferSize = 4;
constexpr uint16_t chunkSizeBufferSize = 4;

// The format structure expected in wav files.
struct WAVEFORMAT
{
    uint16_t FormatTag;        // format type.
    uint16_t Channels;         // number of channels (i.e. mono, stereo...).
    uint32_t SamplesPerSec;    // sample rate.
    uint32_t AvgBytesPerSec;   // for buffer estimation.
    uint16_t BlockAlign;       // block size of data.
    uint16_t BitsPerSample;    // Number of bits per sample of mono data.
} m_formatHeader;
static_assert(sizeof(m_formatHeader) == 16, "unexpected size of m_formatHeader");

void ReadChunkTypeAndSize(std::fstream& fs, char* chunkType, uint32_t* chunkSize)
{
    // Read the chunk type
    fs.read(chunkType, chunkTypeBufferSize);
    
    // Read the chunk size
    uint8_t chunkSizeBuffer[chunkSizeBufferSize];
    fs.read((char*)chunkSizeBuffer, chunkSizeBufferSize);
    
    // chunk size is little endian
    *chunkSize = ((uint32_t)chunkSizeBuffer[3] << 24) |
    ((uint32_t)chunkSizeBuffer[2] << 16) |
    ((uint32_t)chunkSizeBuffer[1] << 8) |
    (uint32_t)chunkSizeBuffer[0];
}

// Get format data from a wav file.
void GetFormatFromWavFile(std::fstream& fs)
{
    char tag[tagBufferSize];
    char chunkType[chunkTypeBufferSize];
    char chunkSizeBuffer[chunkSizeBufferSize];
    uint32_t chunkSize = 0;
    
    // Set to throw exceptions when reading file header.
    fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // Checks the RIFF tag
        fs.read(tag, tagBufferSize);
        if (memcmp(tag, "RIFF", tagBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'RIFF' is expected.");
        
        // The next is the RIFF chunk size, ignore now.
        fs.read(chunkSizeBuffer, chunkSizeBufferSize);
        
        // Checks the 'WAVE' tag in the wave header.
        fs.read(chunkType, chunkTypeBufferSize);
        if (std::memcmp(chunkType, "WAVE", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'WAVE' is expected.");
        
        // The next chunk must be the 'fmt ' chunk.
        ReadChunkTypeAndSize(fs, chunkType, &chunkSize);
        if (std::memcmp(chunkType, "fmt ", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Invalid file header, tag 'fmt ' is expected.");
        
        // Reads format data.
        fs.read((char *)&m_formatHeader, sizeof(m_formatHeader));
        
        // Skips the rest of format data.
        if (chunkSize > sizeof(m_formatHeader))
            fs.seekg(chunkSize - sizeof(m_formatHeader), std::ios_base::cur);
        
        // The next must be the 'data' chunk.
        ReadChunkTypeAndSize(fs, chunkType, &chunkSize);
        if (std::memcmp(chunkType, "data", chunkTypeBufferSize) != 0)
            throw std::runtime_error("Currently the 'data' chunk must directly follow the fmt chunk.");
        if (fs.eof() && chunkSize > 0)
            throw std::runtime_error("Unexpected end of file, before any audio data can be read.");
    }
    catch (std::ifstream::failure e) {
        throw std::runtime_error("Unexpected end of file or error when reading audio file.");
    }
    // Set to not throw exceptions when starting to read audio data, since istream::read() throws exception if the data read is less than required.
    // Instead, in AudioInputStream::Read(), we manually check whether there is an error or not.
    fs.exceptions(std::ifstream::goodbit);
}

@implementation AudioStreamTest

+(void) runPullTest:(NSString *)speechKey
{
    NSBundle *mainBundle = [NSBundle bundleForClass:[self class]];
    NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
    NSLog(@"Main bundle path: %@", mainBundle);
    NSLog(@"weatherFile path: %@", weatherFile);
    NSString *lampFile = [mainBundle pathForResource: @"TurnOnTheLamp" ofType:@"wav"];
    
    std::string audioFileName = [weatherFile UTF8String];
    if (audioFileName.empty())
        throw std::invalid_argument("Audio filename is empty");
    
    __block std::fstream m_fs;
    std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
    m_fs.open(audioFileName, mode);
    if (!m_fs.good())
        throw std::invalid_argument("Failed to open the specified audio file.");
            
    // Get audio format from the file header.
    GetFormatFromWavFile(m_fs);
    
    SPXPullAudioInputStream* stream = [[SPXPullAudioInputStream alloc] initWithReadHandler:
            ^NSInteger(NSMutableData *data, NSUInteger size) {
                if (m_fs.eof()) {
                    // returns 0 to indicate that the stream reaches end.
                    return 0;
                }
                
                char* buffer = (char *)malloc(size);
                m_fs.read((char*)buffer, size);
                int count = (int)m_fs.gcount();
                if (!m_fs.eof() && !m_fs.good()) {
                    // returns 0 to close the stream on read error.
                    NSLog(@"Read error on stream.");
                    count = 0;
                }
                else
                {
                    NSRange range;
                    range.location = 0;
                    range.length = count;
                    [data replaceBytesInRange:range withBytes:buffer];
                    NSLog(@"%d bytes data returned", (int)[data length]);
                }
                free(buffer);
                // returns the number of bytes that have been read.
                return (NSInteger)count;
            }
            closeHandler:
            ^(void) {
                m_fs.close();
            }];
    
    SPXAudioConfiguration* streamAudioSource = [[SPXAudioConfiguration alloc] initWithStreamInput:stream];
    
    __block bool end = false;
    
    SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:@"westus"];
    SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:streamAudioSource];
    
    [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
    }];
    [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
        NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
        end = true;
    }];
    [speechRecognizer addSpeechStartDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addSpeechEndDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
        NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
    }];
    [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
        NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
    }];
    [speechRecognizer addRecognizingEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
        NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
    }];
    
    [speechRecognizer addCanceledEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
        NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
        SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
        NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
    }];
    
    end = false;
    [speechRecognizer startContinuousRecognition];
    while (end == false)
        [NSThread sleepForTimeInterval:1.0f];
    [speechRecognizer stopContinuousRecognition];
    m_fs.close();
}
    
    
+(void) runPushTest:(NSString *)speechKey
    {
        NSBundle *mainBundle = [NSBundle bundleForClass:[self class]];
        NSString *weatherFile = [mainBundle pathForResource: @"whatstheweatherlike" ofType:@"wav"];
        NSLog(@"Main bundle path: %@", mainBundle);
        NSLog(@"weatherFile path: %@", weatherFile);
        NSString *lampFile = [mainBundle pathForResource: @"TurnOnTheLamp" ofType:@"wav"];
        
        std::string audioFileName = [weatherFile UTF8String];
        if (audioFileName.empty()) {
            throw std::invalid_argument("Audio filename is empty");
        }
        
        std::fstream m_fs;
        std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
        m_fs.open(audioFileName, mode);
        if (!m_fs.good())
        throw std::invalid_argument("Failed to open the specified audio file.");
        
        // Get audio format from the file header.
        GetFormatFromWavFile(m_fs);
        
        SPXPushAudioInputStream* pushStream = [[SPXPushAudioInputStream alloc] init];
        
        SPXAudioConfiguration* streamAudioSource = [[SPXAudioConfiguration alloc] initWithStreamInput:pushStream];

        
        __block bool end = false;
        
        SPXSpeechConfiguration *speechConfig = [[SPXSpeechConfiguration alloc] initWithSubscription:speechKey region:@"westus"];
        SPXSpeechRecognizer* speechRecognizer = [[SPXSpeechRecognizer alloc] initWithSpeechConfiguration:speechConfig audioConfiguration:streamAudioSource];
        
        [speechRecognizer addSessionStartedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
            NSLog(@"Received SessionStarted event. SessionId: %@", eventArgs.sessionId);
        }];
        [speechRecognizer addSessionStoppedEventHandler: ^ (SPXRecognizer *recognizer, SPXSessionEventArgs *eventArgs) {
            NSLog(@"Received SessionStopped event. SessionId: %@", eventArgs.sessionId);
            end = true;
        }];
        [speechRecognizer addSpeechStartDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
            NSLog(@"Received SpeechStarted event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [speechRecognizer addSpeechEndDetectedEventHandler: ^ (SPXRecognizer *recognizer, SPXRecognitionEventArgs *eventArgs) {
            NSLog(@"Received SpeechEnd event. SessionId: %@ Offset: %d", eventArgs.sessionId, (int)eventArgs.offset);
        }];
        [speechRecognizer addRecognizedEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
            NSLog(@"Received final result event. SessionId: %@, recognition result:%@. Status %ld.", eventArgs.sessionId, eventArgs.result.text, (long)eventArgs.result.reason);
            NSLog(@"Received JSON: %@", [eventArgs.result.properties getPropertyByName:@"RESULT-Json"]);
        }];
        [speechRecognizer addRecognizingEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionEventArgs *eventArgs) {
            NSLog(@"Received intermediate result event. SessionId: %@, intermediate result:%@.", eventArgs.sessionId, eventArgs.result.text);
        }];
        
        
        [speechRecognizer addCanceledEventHandler: ^ (SPXSpeechRecognizer *recognizer, SPXSpeechRecognitionCanceledEventArgs *eventArgs) {
            NSLog(@"Received canceled event. SessionId: %@, reason:%lu errorDetails:%@.", eventArgs.sessionId, (unsigned long)eventArgs.reason, eventArgs.errorDetails);
            SPXCancellationDetails *details = [[SPXCancellationDetails alloc] initFromCanceledRecognitionResult:eventArgs.result];
            NSLog(@"Received cancellation details. reason:%lu errorDetails:%@.", (unsigned long)details.reason, details.errorDetails);
        }];
        
        
        end = false;
        [speechRecognizer startContinuousRecognition];
        // push stream;
        size_t bytes = 3200;
        char* buffer = (char *)malloc(bytes);
        while (1)
        {
            if (m_fs.eof()) {
                // returns 0 to indicate that the stream reaches end.
                [pushStream close];
                break;
            }
            [NSThread sleepForTimeInterval:0.5f];
            m_fs.read((char*)buffer, bytes);
            int count = (int)m_fs.gcount();
            if (!m_fs.eof() && !m_fs.good()) {
                // returns 0 to close the stream on read error.
                NSLog(@"Read error on stream.");
                [pushStream close];
                break;
            }
            else
            {
                NSData *data = [NSData dataWithBytes:buffer length:count];
                [pushStream write:data];
                NSLog(@"Wrote %d bytes", (int)[data length]);
            }
        }

        while (end == false)
            [NSThread sleepForTimeInterval:1.0f];
        [speechRecognizer stopContinuousRecognition];
        m_fs.close();
    }

@end
