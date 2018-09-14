#pragma once

#include <string>
#include <spxdebug.h>
#include <speechapi_cxx_common.h>
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

    void SetProperty(PropertyId propertyID, const std::string& value)
    {
        property_bag_set_string(m_propbag, (int)propertyID, NULL, value.c_str());
    }

    void SetProperty(const std::string& propertyName, const std::string& value)
    {
        property_bag_set_string(m_propbag, -1, propertyName.c_str(), value.c_str());
    }

    std::string GetProperty(PropertyId propertyID, const std::string& defaultValue = "")
    {
        return property_bag_get_string(m_propbag, static_cast<int>(propertyID), nullptr, defaultValue.c_str());
    }

    std::string GetProperty(const std::string& propertyName, const std::string& defaultValue = "")
    {
        return property_bag_get_string(m_propbag, -1, propertyName.c_str(), defaultValue.c_str());
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
