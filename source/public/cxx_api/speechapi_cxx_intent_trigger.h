//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_trigger.h: Public API declarations for IntentTrigger C++ class
//

#pragma once
#include <speechapi_c.h>
#include <speechapi_cxx_luis_model.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {


class IntentTrigger
{
public:

    static std::shared_ptr<IntentTrigger> From(const wchar_t* simplePhrase)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_Phrase(simplePhrase, &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    static std::shared_ptr<IntentTrigger> From(std::shared_ptr<LuisModel> model)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_LuisModel((SPXLUISHANDLE)(*model.get()), &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    static std::shared_ptr<IntentTrigger> From(std::shared_ptr<LuisModel> model, const wchar_t* intentName)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_LuisModel_Intent((SPXLUISHANDLE)(*model.get()), intentName, &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    IntentTrigger(SPXTRIGGERHANDLE htrigger) : m_htrigger(htrigger) { };
    virtual ~IntentTrigger() { IntentTrigger_Handle_Close(m_htrigger); m_htrigger = SPXHANDLE_INVALID; }

    explicit operator SPXTRIGGERHANDLE() { return m_htrigger; }


private:

    IntentTrigger() = delete;
    IntentTrigger(IntentTrigger&&) = delete;
    IntentTrigger(const IntentTrigger&) = delete;
    IntentTrigger& operator=(IntentTrigger&&) = delete;
    IntentTrigger& operator=(const IntentTrigger&) = delete;

    SPXTRIGGERHANDLE m_htrigger;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent
