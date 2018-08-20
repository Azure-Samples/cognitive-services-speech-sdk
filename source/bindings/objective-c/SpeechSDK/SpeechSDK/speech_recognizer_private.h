//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognizer_private_h
#define speechrecognizer_private_h

@interface SpeechRecognizer (Private)

// Todo: better opaque pointer in obj-c, or a typedef
- (instancetype)init:(void *)recoHandle;

- (void *)getRecoHandle;

- (void)onFinalResultEvent:(SpeechRecognitionResultEventArgs *)eventArgs;
- (void)onIntermediateResultEvent:(SpeechRecognitionResultEventArgs *)eventArgs;

@end
#endif /* speechrecognizer_private_h */
