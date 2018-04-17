//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer.h: Public API declarations for Recognizer C++ base class
//

#pragma once
#include <speechapi_cxx_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Recognition {

/// <summary>
/// Recognizer base class.
/// </summary>
class Recognizer
{
public:

    /// <summary>
    /// Returns true if the recognizer is enabled.
    /// </summary>
    virtual bool IsEnabled()
    {
        bool enabled = false;
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_IsEnabled(m_hreco, &enabled));
        return enabled;
    };

    /// <summary>
    /// Enables the recognizer.
    /// </summary>
    virtual void Enable()
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Enable(m_hreco));
    };

    /// <summary>
    /// Disables the recognizer.
    /// </summary>
    virtual void Disable()
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Disable(m_hreco));
    };

protected:

    Recognizer(SPXRECOHANDLE hreco) :
        m_hreco(hreco)
    {
        SPX_DBG_TRACE_FUNCTION();
    }

    virtual ~Recognizer()
    {
        SPX_DBG_TRACE_FUNCTION();

        if (m_hreco != SPXHANDLE_INVALID)
        {
            ::Recognizer_Handle_Close(m_hreco);
            m_hreco = SPXHANDLE_INVALID;
        }
    }

    SPXRECOHANDLE m_hreco;

private:

    DISABLE_DEFAULT_CTORS(Recognizer);
};


} } } } // Microsoft::CognitiveServices::Speech::Recognition
