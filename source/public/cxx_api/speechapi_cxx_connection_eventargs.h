//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_session_eventargs.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Provides data for the ConnectionEvent.
/// Added in version 1.2.0.
/// </summary>
class ConnectionEventArgs : public SessionEventArgs
{
public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle.</param>
    explicit ConnectionEventArgs(SPXEVENTHANDLE hevent) :
        SessionEventArgs(hevent)
    {
    };

    /// <inheritdoc/>
    virtual ~ConnectionEventArgs() {}

private:

    DISABLE_COPY_AND_MOVE(ConnectionEventArgs);
};


} } } // Microsoft::CognitiveServices::Speech
