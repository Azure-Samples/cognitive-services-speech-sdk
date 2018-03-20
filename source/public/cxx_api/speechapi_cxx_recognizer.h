//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer.h: Public API declarations for Recognizer C++ base class
//

#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class Recognizer
{
public:

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

    virtual bool IsEnabled()
    {
        bool enabled = false;
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_IsEnabled(m_hreco, &enabled));
        return enabled;
    };

    virtual void Enable()
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Enable(m_hreco));
    };

    virtual void Disable()
    {
        SPX_INIT_HR(hr);
        SPX_THROW_ON_FAIL(hr = Recognizer_Disable(m_hreco));
    };


protected:

    SPXRECOHANDLE m_hreco;


private:

    Recognizer() = delete;
    Recognizer(Recognizer&&) = delete;
    Recognizer(const Recognizer&) = delete;
    Recognizer& operator=(Recognizer&&) = delete;
    Recognizer& operator=(const Recognizer&) = delete;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
