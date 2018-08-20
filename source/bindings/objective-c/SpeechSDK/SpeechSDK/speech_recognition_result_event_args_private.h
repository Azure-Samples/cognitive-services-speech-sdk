//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef speechrecognitioneventargs_private_h
#define speechrecognitioneventargs_private_h

@interface SpeechRecognitionResultEventArgs (Private)

// Todo: better opaque pointer in obj-c, or a typedef
- (instancetype)init: (void *) eventArgsHandle;

@end

#endif /* speechrecognitioneventargs_private_h */
