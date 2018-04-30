//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_session_eventargs.h: Public API declarations for SessionEventArgs C++ class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_c_recognizer.h>
#include <speechapi_cxx_eventargs.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
   
    
/// <summary>
/// Base class for session event arguments.
/// </summary>
class SessionEventArgs : public EventArgs
{
public:

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="hevent">Event handle</param>
    explicit SessionEventArgs(SPXEVENTHANDLE hevent) :
        SessionId(m_sessionId),
        m_sessionId(GetSessionId(hevent))
    {
    };

    /// <inheritdoc/>
    virtual ~SessionEventArgs() {}

    /// <summary>
    /// Session identifier (a GUID in string format).
    /// </summary>
    const std::wstring& SessionId;


protected:

    /*! \cond PROTECTED */

    /// <summary>
    /// Extract session identifier from given event handle <paramref name="hevent"/>
    /// </summary>
    static const std::wstring GetSessionId(SPXEVENTHANDLE hevent)
    {
        static const auto cchMaxUUID = 36;
        static const auto cchMaxSessionId = cchMaxUUID + 1;
        wchar_t sessionId[cchMaxSessionId] = {};

        SPX_THROW_ON_FAIL(Recognizer_SessionEvent_GetSessionId(hevent, sessionId, cchMaxSessionId));
        return std::wstring(sessionId);
    };

    /*! \endcond */

private:

    DISABLE_DEFAULT_CTORS(SessionEventArgs);

    std::wstring m_sessionId;
};


} } } // Microsoft::CognitiveServices::Speech
