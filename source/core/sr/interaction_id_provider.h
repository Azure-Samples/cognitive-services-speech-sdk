//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interaction_id_provider.h: Declarations for CSpxInteractionIdProvider C++ class
//

#pragma once

#include <string>
#include <mutex>

#include "ispxinterfaces.h"
#include "guid_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/**
 * This class is a helper that implements the ISpxInteractionProvider, it is meant to be used by inheriting from it.
 * In a DialogConnector flow NextInteractionId needs to be called at the beggining of an interaction (i.e Speech, Activity),
 * and after that GetInteractionId should be called to get the current interaction id.
 * note: All of this rests on the fact that there is only one interaction of each type at the same time.
 */
template<typename T>
class CSpxInteractionIdProvider:
    public ISpxInteractionIdProvider
{
public:
    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxInteractionIdProvider)
    SPX_INTERFACE_MAP_END()

    inline virtual ~CSpxInteractionIdProvider() {}

    inline std::string NextInteractionId(InteractionIdPurpose purpose) noexcept final
    {
        std::lock_guard<std::mutex> lk{ m_lock };
        auto&& iid = Select(purpose);
        return iid.Next();
    }

    inline std::string GetInteractionId(InteractionIdPurpose purpose) const noexcept final
    {
        std::lock_guard<std::mutex> lk{ m_lock };
        auto&& iid = Select(purpose);
        return iid.Get();
    }

private:
    friend T;

    inline CSpxInteractionIdProvider()
    {}

    struct InteractionId
    {
        std::string m_id;

        inline std::string Next() noexcept
        {
            m_id = PAL::CreateGuidWithDashesUTF8();
            return m_id;
        }

        inline std::string Get() const noexcept
        {
            return m_id;
        }
    };

    inline InteractionId& Select(InteractionIdPurpose purpose) noexcept
    {
        if (purpose == InteractionIdPurpose::Speech)
        {
            return m_speech;
        }
        return m_activity;
    }

    inline const InteractionId& Select(InteractionIdPurpose purpose) const noexcept
    {
        return const_cast<CSpxInteractionIdProvider*>(this)->Select(purpose);
    }

    mutable std::mutex m_lock;

    InteractionId m_speech;
    InteractionId m_activity;

};

} } } }
