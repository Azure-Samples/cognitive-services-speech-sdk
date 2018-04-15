//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_intent_trigger.h: Public API declarations for IntentTrigger C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_c.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_luis_model.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {
namespace Intent {

/// <summary>
/// Represents an intent trigger.
/// </summary>
class IntentTrigger
{
public:

    /// <summary>
    /// Creates an intent trigger using the specified phrase.
    /// </summary>
    /// <param name="simplePhrase">The simple phrase to create an intent trigger for.</param>
    /// <returns>A shared pointer to an intent trigger.</returns>
    static std::shared_ptr<IntentTrigger> From(const wchar_t* simplePhrase)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_Phrase(simplePhrase, &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    /// <summary>
    /// Creates an intent trigger using the specified LuisModel.
    /// </summary>
    /// <param name="model">The LuisModel to create an intent trigger for.</param>
    /// <returns>A shared pointer to an intent trigger.</returns>
    static std::shared_ptr<IntentTrigger> From(std::shared_ptr<LuisModel> model)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_LuisModel((SPXLUISHANDLE)(*model.get()), &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    /// <summary>
    /// Creates an intent trigger using the specified LuisModel and an intent name.
    /// </summary>
    /// <param name="model">The LuisModel to create an intent trigger for.</param>
    /// <param name="model">The intent name to create an intent trigger for.</param>
    /// <returns>A shared pointer to an intent trigger.</returns>
    static std::shared_ptr<IntentTrigger> From(std::shared_ptr<LuisModel> model, const wchar_t* intentName)
    {
        SPXTRIGGERHANDLE htrigger = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(IntentTrigger_Create_From_LuisModel_Intent((SPXLUISHANDLE)(*model.get()), intentName, &htrigger));
        return std::make_shared<IntentTrigger>(htrigger);
    }

    /// <summary>
    /// Virtual destructor
    /// </summary>
    virtual ~IntentTrigger() { IntentTrigger_Handle_Close(m_htrigger); m_htrigger = SPXHANDLE_INVALID; }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    IntentTrigger(SPXTRIGGERHANDLE htrigger) : m_htrigger(htrigger) { };

    /// <summary>
    /// Internal. Explicit conversion operator.
    /// </summary>
    explicit operator SPXTRIGGERHANDLE() { return m_htrigger; }

private:
    DISABLE_DEFAULT_CTORS(IntentTrigger);

    SPXTRIGGERHANDLE m_htrigger;
};


} } } // CARBON_NAMESPACE_ROOT :: Recognition :: Intent
