//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_participant.h: Public API declarations for Participant C++ class
//

#pragma once
#include <speechapi_c.h>
#include <speechapi_cxx_properties.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Transcription {

/// <summary>
/// Represents a participant in a conversation.
/// Added in version 1.5.0.
/// </summary>
class Participant
{
public:

    /// <summary>
    /// Create a participant using user id, her/his preferred language and her/his voice signature.
    /// If voice signature is empty then user will not be identified.
    /// </summary>
    /// <param name="userId">A user ids.</param>
    /// <param name="preferredLanguage">The preferred languages of the user. It can be optional.</param>
    /// <param name="voiceSignature">The voice signature of the user. It can be optional.</param>
    /// <returns>A smart pointer of Participant </returns>
    static std::shared_ptr<Participant> From(const SPXSTRING& userId, const SPXSTRING& preferredLanguage = {}, const SPXSTRING& voiceSignature = {})
    {
        SPXPARTICIPANTHANDLE hparticipant = SPXHANDLE_INVALID;
        SPX_THROW_ON_FAIL(participant_create_handle(&hparticipant, Utils::ToUTF8(userId.c_str()), Utils::ToUTF8(preferredLanguage.c_str()), Utils::ToUTF8(voiceSignature.c_str())));
        return std::make_shared<Participant>(hparticipant);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hparticipant">participant handle.</param>
    explicit Participant(SPXPARTICIPANTHANDLE hparticipant = SPXHANDLE_INVALID) :
        m_properties(hparticipant),
        Properties(m_properties),
        m_hparticipant(hparticipant)
    {}

    /// <summary>
    /// Virtual destructor.
    /// </summary>
    virtual ~Participant() { participant_release_handle(m_hparticipant); }

    /// <summary>
    /// Internal operator used to get underlying handle value.
    /// </summary>
    /// <returns>A handle.</returns>
    explicit operator SPXPARTICIPANTHANDLE() const { return m_hparticipant; }

    /// <summary>
    /// Set preferred language.
    /// </summary>
    /// <param name="preferredLanguage">The preferred language, such as "en-us".</param>
    void SetPreferredLanguage(const std::string& preferredLanguage)
    {
        SPX_THROW_ON_FAIL(participant_set_preferred_langugage(m_hparticipant, Utils::ToUTF8(preferredLanguage.c_str())));
    }

    /// <summary>
    /// Set voice signature.
    /// If voice signature is empty then user will not be identified.
    /// </summary>
    /// <param name="voiceSignature">The participant's voice signature."</param>
    void SetVoiceSignature(const std::string& voiceSignature)
    {
        SPX_THROW_ON_FAIL(participant_set_voice_signature(m_hparticipant, Utils::ToUTF8(voiceSignature.c_str())));
    }

private:

    /*! \cond PRIVATE */

    class PrivatePropertyCollection : public PropertyCollection
    {
    public:
        PrivatePropertyCollection(SPXPARTICIPANTHANDLE hparticipant) :
            PropertyCollection(
                [=]() {
            SPXPROPERTYBAGHANDLE hpropbag = SPXHANDLE_INVALID;
            participant_get_property_bag(hparticipant, &hpropbag);
            return hpropbag;
        }())
        {
        }
    };

    PrivatePropertyCollection m_properties;

    /*! \endcond */

public:

    /// <summary>
    /// Collection of additional participant properties.
    /// </summary>
    PropertyCollection& Properties;

private:
    DISABLE_COPY_AND_MOVE(Participant);

    SPXPARTICIPANTHANDLE m_hparticipant;
};

}}}}
