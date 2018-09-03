//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognitionresult_private_h
#define speechrecognitionresult_private_h

#define BINDING_OBJECTIVE_C

@interface SpeechRecognitionResult (Private)

// Todo: better opaque pointer in obj-c, or a typedef
- (instancetype)init:(void *)resultHandle;

- (instancetype)initWithRuntimeError:(NSString *)message;

@end

#endif /* speechrecognitionresult_private_h */
