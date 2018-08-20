//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#import <Foundation/Foundation.h>
#import "speech_recognition_result_event_args.h"
#import "speech_recognition_result_event_args_private.h"
#import "speech_recognition_result_private.h"
#import "../../../../public/cxx_api/speechapi_cxx.h"

@implementation SpeechRecognitionResultEventArgs
{
    Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs* eventArgsImpl;
}

- (instancetype)init:(void *)eventArgsHandle
{
    eventArgsImpl = static_cast<Microsoft::CognitiveServices::Speech::SpeechRecognitionEventArgs *>(eventArgsHandle);
    
    std::wstring stdwstring = eventArgsImpl->SessionId;
    char *data = (char *)stdwstring.data();
    size_t size = stdwstring.size() * sizeof(wchar_t);
    _sessionId = [[NSString alloc] initWithBytes:data length:size encoding:CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE)];
    
    // Todo: error handling
    std::shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognitionResult> resultImpl = eventArgsImpl->GetResult();
    _result = [[SpeechRecognitionResult alloc] init: (void *)&resultImpl];
    
    return self;
}

@end

