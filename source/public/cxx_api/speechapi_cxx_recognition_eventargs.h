//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_eventargs.h: Public API declarations for RecognitionEventArgs C++ base class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Recognition {

/// <summary>
/// Provides data for the RecognitionEvent.
/// </summary>
class RecognitionEventArgs : public SessionEventArgs
{
public:

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    explicit RecognitionEventArgs(SPXEVENTHANDLE hevent = SPXHANDLE_INVALID) :
        SessionEventArgs(hevent)
    {
    };

    // Todo: make it private after passing SWIG
public:
    DISABLE_COPY_AND_MOVE(RecognitionEventArgs);
};


} } } } // Microsoft::CognitiveServices::Speech::Recognition
