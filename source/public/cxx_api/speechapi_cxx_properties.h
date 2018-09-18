#pragma once

#include <string>
#include <spxdebug.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_c_session.h>
#include <speechapi_c_property_bag.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


class PropertyCollection
{
public:

    ~PropertyCollection()
    {
        if (property_bag_is_valid(m_propbag))
        {
            property_bag_release(m_propbag);
            m_propbag = SPXHANDLE_INVALID;
        }
    }

    void SetProperty(PropertyId propertyID, const SPXSTRING& value)
    {
        property_bag_set_string(m_propbag, (int)propertyID, NULL, Utils::ToUTF8(value).c_str());
    }

    void SetProperty(const SPXSTRING& propertyName, const SPXSTRING& value)
    {
        property_bag_set_string(m_propbag, -1, Utils::ToUTF8(propertyName).c_str(), Utils::ToUTF8(value).c_str());
    }

    SPXSTRING GetProperty(PropertyId propertyID, const SPXSTRING& defaultValue = SPXSTRING())
    {
        return Utils::ToSPXString(property_bag_get_string(m_propbag, static_cast<int>(propertyID), nullptr, Utils::ToUTF8(defaultValue).c_str()));
    }

    SPXSTRING GetProperty(const SPXSTRING& propertyName, const SPXSTRING& defaultValue = SPXSTRING())
    {
        return Utils::ToSPXString(property_bag_get_string(m_propbag, -1, Utils::ToUTF8(propertyName).c_str(), Utils::ToUTF8(defaultValue).c_str()));
    }

protected:

    /*! \cond PROTECTED */

    PropertyCollection(SPXPROPERTYBAGHANDLE propbag) : m_propbag(propbag) {}

    /*! \endcond */

private:

    DISABLE_COPY_AND_MOVE(PropertyCollection);

    SPXPROPERTYBAGHANDLE m_propbag;
};


} } } // Microsoft::CognitiveServices::Speech
