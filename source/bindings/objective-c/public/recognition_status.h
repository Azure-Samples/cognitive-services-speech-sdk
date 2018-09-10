//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#ifndef recognition_status_h
#define recognition_status_h

typedef NS_ENUM(NSInteger, RecognitionStatus)
{
    Recognized = 0,
    IntermediateResult,
    NoMatch,
    InitialSilenceTimeout,
    InitialBabbleTimeout,
    Canceled
};

#endif /* recognition_status_h */
