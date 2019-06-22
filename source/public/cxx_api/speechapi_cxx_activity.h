//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <type_traits>

#include <speechapi_cxx_common.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_cxx_utils.h>
#include <speechapi_c.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Dialog {

/*! \cond PRIVATE */

// Internally used string constants.

namespace ActivityFields
{
    namespace Fields
    {
        namespace ReadOnly
        {
            static const std::string Timestamp{ "timestamp" };
            static const std::string LocalTimestamp{ "localTimestamp" };
            static const std::string LocalTimezone{ "localTimezone" };
            static const std::string ChannelId{ "channelId" };
        }
        namespace Properties
        {
            static const std::string Type{ "type" };
            static const std::string Id{ "id" };
            static const std::string ReplyToId{ "replyToId" };
            static const std::string Text{ "text" };
            static const std::string TextFormat{ "textFormat" };
            static const std::string Locale{ "locale" };
            static const std::string Speak{ "speak" };
            static const std::string InputHint{ "inputHint" };
            static const std::string AttachmentLayout{ "attachmentLayout" };
            static const std::string Summary{ "summary" };
            static const std::string Expiration{ "expiration" };
            static const std::string Importance{ "importance" };
            static const std::string DeliveryMode{ "deliveryMode" };
        }
        namespace Complex
        {
            static const std::string From{ "from" };
            static const std::string Recipient{ "recipient" };
            static const std::string Conversation{ "conversation" };
            static const std::string ChannelData{ "channelData" };
            static const std::string SuggestedActions{ "suggestedActions" };
            static const std::string Value{ "value" };
            static const std::string SemanticAction{ "SemanticAction" };
        }
        namespace List
        {
            static const std::string Entities{ "entities" };
            static const std::string Attachments{ "attachments" };
        }
    }
}

/*! \endcond */

/// <summary>
/// Class that represents an activity.
/// </summary>
class Activity : public std::enable_shared_from_this<Activity>
{
protected:
    /*! \cond PROTECTED */
    SPXACTIVITYHANDLE m_handle;
    /*! \endcond */
public:
#ifndef SWIG
    /// <summary>
    /// Class that represents an activity property.
    /// </summary>
    /// <typeparam name="T">Property type</typeparam>
    /// <typeparam name="isReadOnly">If the property is read only.</typeparam>
    template<typename T, bool isReadOnly = false>
    class Property
    {
    public:
        /// <summary>
        /// Gets a reference to the property value.
        /// </summary>
        /// <returns>Constant reference to the property value</returns>
        inline const T& Get() const
        {
            return m_cache;
        }

        /// <summary>
        /// Sets the property value.
        /// </summary>
        /// <param name="v">Value to set</param>
        /// <returns>A const reference to the property value.</returns>
        template<typename V, typename = std::enable_if_t<std::is_constructible<T, V>::value>>
        inline std::enable_if_t<!isReadOnly, const T&> Set(V&& v)
        {
            m_cache = { std::forward<T>(v) };
            m_put(m_cache);
            return m_cache;
        }

        /// <summary>
        /// Sets the property value.
        /// </summary>
        /// <param name="v">Value to set</param>
        /// <returns>A const reference to the property value.</returns>
        template<typename V, typename = std::enable_if_t<std::is_constructible<T, V>::value>>
        inline std::enable_if_t<!isReadOnly, const T&> operator=(V&& v)
        {
            return Set(std::forward<V>(v));
        }

        /// <summary>
        /// Implicit conversion to the property type.
        /// </summary>
        /// <returns>Constant reference to the property value</returns>
        operator const T&() const
        {
            return Get();
        }

        /// <summary>
        /// Equality operator.
        /// </summary>
        /// <param name="rhs">Property object</param>
        /// <returns>true if the value of the object is equal to the value contained in the property.</returns>
        bool operator==(const T& rhs) const
        {
            return Get() == rhs;
        }

        /// <summary>
        /// Inequality operator.
        /// </summary>
        /// <param name="rhs">Property object</param>
        /// <returns>true if the value of the object is not equal to the value contained in the property.</returns>
        bool operator!=(const T& rhs) const
        {
            return !(Get() == rhs);
        }

        /// <summary>
        /// Equality operator.
        /// </summary>
        /// <param name="lhs">Object of the same type as the property</param>
        /// <param name="rhs">Property object</param>
        /// <returns>true if the value of the object is equal to the value contained in the property.</returns>
        friend bool operator==(const T& lhs, const Property& rhs)
        {
            return lhs == rhs.Get();
        }

        /// <summary>
        /// Inequality operator.
        /// </summary>
        /// <param name="lhs">Object of the same type as the property</param>
        /// <param name="rhs">Property object</param>
        /// <returns>true if the value of the object is not equal to the value contained in the property.</returns>
        friend bool operator!=(const T& lhs, const Property& rhs)
        {
            return lhs == rhs.Get();
        }

        /// <summary>
        /// Convenience overload for stream insertion.
        /// </summary>
        /// <param name="stream">Stream being operated.</param>
        /// <param name="property">Property object to add to the stream.</param>
        /// <returns>Stream being used.</returns>
        friend std::ostream& operator<<(std::ostream& stream, const Property& property)
        {
            return stream << property.m_cache;
        }

        friend class Activity;
    private:
        /*! \cond PROTECTED */
        using get_fn = std::function<T()>;
        using put_fn = std::function<void(const T&)>;

        Property(get_fn get) : m_get{ get }
        {}

        Property(get_fn get, put_fn put) : m_get{ get }, m_put{ put }, m_cache{ m_get() }
        {}

        get_fn m_get;
        put_fn m_put;
        T m_cache;
        /*! \endcond */
    };

    using string_prop = Property<std::string>;
    using ro_string_prop = Property<std::string>;

    /// <summary>
    /// Class that represents a JSON object.
    /// </summary>
    class JSON
    {
    public:
        using valid_types = Utils::TypeList<double, uint64_t, int64_t, std::string, bool>;

        /// <summary>Checks if the JSON is of the type Null</summary>
        /// <returns>Whether the object is null</returns>
        inline bool IsNull() const
        {
            return check_type<ActivityJSONType::Null>();
        }

        /// <summary>Checks if the JSON is a JSON object</summary>
        /// <returns>Whether the object is an object</returns>
        inline bool IsObject() const
        {
            return check_type<ActivityJSONType::Object>();
        }

        /// <summary>Checks if the JSON is of the type double</summary>
        /// <returns>Whether the object is a double</returns>
        bool IsDouble() const
        {
            return check_type<ActivityJSONType::Double>();
        }

        /// <summary>Checks if the JSON is of the type unsigned int</summary>
        /// <returns>Whether the object is an unsigned integer</returns>
        bool IsUInt() const
        {
            return check_type<ActivityJSONType::UInt>();
        }

        /// <summary>Checks if the JSON is of the type signed integer</summary>
        /// <returns>Whether the object is a signed integer</returns>
        bool IsInt() const
        {
            return check_type<ActivityJSONType::Int>();
        }

        /// <summary>Checks if the JSON is of the type string</summary>
        /// <returns>Whether the object is a string</returns>
        bool IsString() const
        {
            return check_type<ActivityJSONType::String>();
        }

        /// <summary>Checks if the JSON corresponds to a JSON array</summary>
        /// <returns>Whether the object is an array</returns>
        bool IsArray() const
        {
            return check_type<ActivityJSONType::Array>();
        }

        /*! \cond PROTECTED */
        JSON(const JSON&) = delete;

        JSON(JSON&& other) : m_handle{ other.m_handle }, m_fields{ std::move(other.m_fields) }, m_items{ std::move(other.m_items) }
        {
            other.m_handle = 0;
        }
        ~JSON()
        {
            if (m_handle != SPXHANDLE_INVALID)
            {
                activity_json_handle_release(m_handle);
            }
        }
        /*! \endcond */

        /// <summary>
        /// Checks if the JSON contains the given field.
        /// This is valid for objects of type Object.
        /// </summary>
        /// <param name="field">Field to check.</param>
        /// <returns>Whether the object contains the field</returns>
        bool Has(const std::string& field)
        {
            bool has_field{ false };
            SPX_THROW_ON_FAIL(activity_json_has_field(m_handle, field.c_str(), &has_field));
            return has_field;
        }

        /// <summary>
        /// Gets a reference to the specified field.
        /// This is valid for objects of type Object.
        /// </summary>
        /// <param name="field">Field to get a reference to.</param>
        /// <returns>A reference to the field</returns>
        JSON& operator[](const std::string& field)
        {
            auto search = m_fields.find(field);
            if (search == m_fields.end())
            {
                /* w007, a new field! */
                SPXACTIVITYJSONHANDLE h;
                SPX_THROW_ON_FAIL(activity_json_field_handle(m_handle, field.c_str(), &h));
                auto r = m_fields.insert(std::make_pair(field, JSON{ h }));
                return r.first->second;
            }
            return search->second;
        }

        /// <summary>
        /// Assigns a value to the object.
        /// This is valid for objects of value types.
        /// </summary>
        /// <param name="val">Value to assign.</param>
        template<typename T, typename = std::enable_if_t<Utils::TypeListIfAny<T, std::is_constructible, valid_types>::value> >
        void operator=(T&& val)
        {
            SetValue(val);
        }

        /// <summary>
        /// Assigns a value to the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <param name="val">Value to assign.</param>
        void operator=(const char *val)
        {
            /* Template deduction guides would probably remove the need for this overload */
            return operator=(std::string{ val });
        }

        /// <summary>
        /// Gets the value contained in the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <typeparam name="T">Type of the value to get.</typeparam>
        /// <returns>The value contained in the object</returns>
        template<typename T, typename = std::enable_if_t<std::is_same<std::string, T>::value>>
        T Get() const
        {
            /* When C++17 comes this will be nicer, all these Get methods can be merged with constexpr if */
            std::size_t size{ 0 };
            SPX_THROW_ON_FAIL(activity_json_get_string_size(m_handle, &size));
            auto buffer = std::make_unique<char[]>(size + 1);
            SPX_THROW_ON_FAIL(activity_json_get_string(m_handle, buffer.get(), size + 1));
            return buffer.get();
        }

        /// <summary>
        /// Gets the value contained in the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <typeparam name="T">Type of the value to get.</typeparam>
        /// <returns>The value contained in the object</returns>
        template<typename T, typename = std::enable_if_t<std::is_same<double, T>::value>, uint8_t = uint8_t{}>
        T Get() const
        {
            double dbl;
            SPX_THROW_ON_FAIL(activity_json_get_double(m_handle, &dbl));
            return dbl;
        }

        /// <summary>
        /// Gets the value contained in the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <typeparam name="T">Type of the value to get.</typeparam>
        /// <returns>The value contained in the object</returns>
        template<typename T, typename = std::enable_if_t<std::is_same<bool, T>::value>, bool = bool{}>
        T Get() const
        {
            bool bl;
            SPX_THROW_ON_FAIL(activity_json_get_bool(m_handle, &bl));
            return bl;
        }

        /// <summary>
        /// Gets the value contained in the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <typeparam name="T">Type of the value to get.</typeparam>
        /// <returns>The value contained in the object</returns>
        template<typename T, typename = std::enable_if_t<std::is_same<int64_t, T>::value>, int64_t = int64_t{} >
        T Get() const
        {
            int64_t i;
            SPX_THROW_ON_FAIL(activity_json_get_int(m_handle, &i));
            return i;
        }

        /// <summary>
        /// Gets the value contained in the object.
        /// This is valid for objects of value types
        /// </summary>
        /// <typeparam name="T">Type of the value to get.</typeparam>
        /// <returns>The value contained in the object</returns>
        template<typename T, typename = std::enable_if_t<std::is_same<uint64_t, T>::value>, uint64_t = uint64_t{} >
        T Get() const
        {
            uint64_t ui;
            SPX_THROW_ON_FAIL(activity_json_get_uint(m_handle, &ui));
            return ui;
        }

        /// <summary>
        /// Gets a reference to the list item with the specified index.
        /// This is valid for objects of type array
        /// </summary>
        /// <param name="idx">Index of the item to get.</param>
        /// <returns>A reference to the item.</returns>
        JSON& operator[](std::size_t idx)
        {
            if (m_items.size() < (idx + 1))
            {
                /* Not in cache, fetch of insert */
                SPXACTIVITYJSONHANDLE h;
                SPX_THROW_ON_FAIL(activity_json_array_item(m_handle, idx, &h));
                m_items.push_back(JSON{ h });
            }
            return m_items[idx];
        }

        /// <summary>
        /// Adds a new empty item to the list and returns a reference to it.
        /// This is valid for objects of type array
        /// </summary>
        /// <returns>A reference to the added item.</returns>
        JSON& Add()
        {
            std::size_t idx = Size();
            return operator[](idx);
        }

        /// <summary>
        /// Gets the size of the array.
        /// This is valid for objects of type array
        /// </summary>
        /// <returns>The size of the array.</returns>
        std::size_t Size() const
        {
            std::size_t size{};
            SPX_THROW_ON_FAIL(activity_json_array_size(m_handle, &size));
            return size;
        }

        friend class Activity;
    private:
        /*! \cond PROTECTED */
        template<ActivityJSONType t>
        bool check_type() const
        {
            ActivityJSONType type;
            activity_json_get_type(m_handle, reinterpret_cast<int*>(&type));
            return t == type;
        }


        JSON(SPXACTIVITYJSONHANDLE handle) : m_handle{ handle }
        {
        }

        template<typename T, std::enable_if_t<std::is_floating_point<T>::value && !std::is_integral<T>::value, int> = 0>
        void SetValue(T val)
        {
            activity_json_set_double(m_handle, double{ val });
        }

        template<typename T, std::enable_if_t<std::is_same<bool, std::decay_t<T>>::value, int> = 0>
        void SetValue(T val)
        {
            activity_json_set_bool(m_handle, bool{ val });
        }

        template<typename T, std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value && !std::is_same<bool, std::decay_t<T>>::value, int> = 0>
        void SetValue(T val)
        {
            activity_json_set_uint(m_handle, uint64_t{ val });
        }

        template<typename T, std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, int> = 0>
        void SetValue(T val)
        {
            activity_json_set_int(m_handle, int64_t{ val });
        }

        void SetValue(const std::string& val)
        {
            activity_json_set_string(m_handle, val.c_str());
        }

        SPXACTIVITYJSONHANDLE m_handle;
        std::map<std::string, JSON> m_fields;
        std::vector<JSON> m_items;
        /*! \endcond */
    };

    /// <summary>
    /// Class that represents a JSON array
    /// </summary>
    struct JSONArray
    {
        JSONArray(const JSONArray&) = delete;
        JSONArray(JSONArray&& other) : m_json{ std::move(other.m_json) }
        {}

        /// <summary>
        /// Gets a reference to the item in the specified index.
        /// </summary>
        /// <param name="idx">Index of the item to get.</param>
        /// <returns>Reference to the item.</returns>
        JSON& operator[](std::size_t idx)
        {
            return m_json[idx];
        }

        /// <summary>
        /// Adds a new empty item to the list and returns a reference to it.
        /// </summary>
        /// <returns>A reference to the added item.</returns>
        JSON& Add()
        {
            return m_json.Add();
        }

        /// <summary>
        /// Gets the size of the array.
        /// </summary>
        /// <returns>The size of the array.</returns>
        std::size_t Size() const
        {
            return m_json.Size();
        }

        friend class Activity;
    private:
        JSONArray(JSON json) : m_json{ std::move(json) }
        {
        }

        JSON m_json;
    };
#endif
public:
    friend class DialogServiceConnector;
    friend class ActivityReceivedEventArgs;

#ifndef SWIG
    /// <summary>
    /// Creates an empty activity instance.
    /// </summary>
    /// <returns>Newly created instance.</returns>
    static inline std::shared_ptr<Activity> Create()
    {
        return std::shared_ptr<Activity>(new Activity{});
    }
#endif

    /// <summary>
    /// Create an activity from a serialized activity.
    /// </summary>
    /// <param name="activity">Serialized activity</param>
    /// <returns>Shared pointer to the created activity</returns>
    static inline std::shared_ptr<Activity> FromSerializedActivity(const std::string& activity)
    {
        SPXACTIVITYHANDLE h;
        SPX_THROW_ON_FAIL(activity_from_string(activity.c_str(), &h));
        return std::shared_ptr<Activity>(new Activity{ h });
    }

    /// <summary>
    /// Returns the activity serialized as a string.
    /// </summary>
    /// <returns>The serialized activity.</returns>
    inline std::string Serialize()
    {
        std::size_t size{ 0 };
        SPX_THROW_ON_FAIL(activity_serialized_size(m_handle, &size));
        auto buffer = std::make_unique<char[]>(size + 1);
        SPX_THROW_ON_FAIL(activity_serialize(m_handle, buffer.get(), size + 1));
        return std::string{ buffer.get() };
    }

#ifndef SWIG
    ro_string_prop GetTimestamp;
    ro_string_prop GetLocalTimestamp;
    ro_string_prop GetLocalTimezone;
    ro_string_prop GetChannelId;

    string_prop Type;
    string_prop Id;
    JSON& From()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::From, m_complex_fields);
    }

    JSON& Recipient()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::Recipient, m_complex_fields);
    }

    JSON& Conversation()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::Conversation, m_complex_fields);
    }

    string_prop ReplyToId;

    JSONArray& Entities()
    {
        return complex_field<JSONArray>(ActivityFields::Fields::List::Entities, m_array_fields);
    }

    JSON& ChannelData()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::ChannelData, m_complex_fields);
    }

    /* This member function provides access to a JSON which fields will be added at the root level of the activity */
    /*JSON& ExtraFields()
    {
        return complex_field<JSON>(Complex::ExtraFields, m_complex_fields);
    }*/

    /* Message Activity Fields */
    string_prop Text;
    string_prop TextFormat;
    string_prop Locale;
    string_prop Speak;
    string_prop InputHint;
    JSONArray& Attachments()
    {
        return complex_field<JSONArray>(ActivityFields::Fields::List::Attachments, m_array_fields);
    }
    string_prop AttachmentLayout;
    string_prop Summary;
    JSON& SuggestedActions()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::SuggestedActions, m_complex_fields);
    }

    JSON& Value()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::Value, m_complex_fields);
    }

    string_prop Expiration;
    string_prop Importance;
    string_prop DeliveryMode;
    JSON& SemanticAction()
    {
        return complex_field<JSON>(ActivityFields::Fields::Complex::SemanticAction, m_complex_fields);
    }
#endif
private:
    /*! \cond PROTECTED */
#ifndef SWIG
    Activity(SPXACTIVITYHANDLE handle) : m_handle{ handle },
        GetTimestamp{ build_ro_prop(ActivityFields::Fields::ReadOnly::Timestamp) },
        GetLocalTimestamp{ build_ro_prop(ActivityFields::Fields::ReadOnly::LocalTimestamp) },
        GetLocalTimezone{ build_ro_prop(ActivityFields::Fields::ReadOnly::LocalTimezone) },
        GetChannelId{ build_ro_prop(ActivityFields::Fields::ReadOnly::ChannelId) },
        Type{ build_prop(ActivityFields::Fields::Properties::Type) },
        Id{ build_prop(ActivityFields::Fields::Properties::Id) },
        ReplyToId{ build_prop(ActivityFields::Fields::Properties::ReplyToId) },
        Text{ build_prop(ActivityFields::Fields::Properties::Text) },
        TextFormat{ build_prop(ActivityFields::Fields::Properties::TextFormat) },
        Locale{ build_prop(ActivityFields::Fields::Properties::Locale) },
        Speak{ build_prop(ActivityFields::Fields::Properties::Speak) },
        InputHint{ build_prop(ActivityFields::Fields::Properties::InputHint) },
        AttachmentLayout{ build_prop(ActivityFields::Fields::Properties::AttachmentLayout) },
        Summary{ build_prop(ActivityFields::Fields::Properties::Summary) },
        Expiration{ build_prop(ActivityFields::Fields::Properties::Expiration) },
        Importance{ build_prop(ActivityFields::Fields::Properties::Importance) },
        DeliveryMode{ build_prop(ActivityFields::Fields::Properties::DeliveryMode) }
    {}
#else
    Activity(SPXACTIVITYHANDLE handle) : m_handle{ handle }
    {}
#endif

    Activity() : Activity{ create_instance() }
    {}

#ifndef SWIG
    std::map<std::string, JSON> m_complex_fields;
    std::map<std::string, JSONArray> m_array_fields;
#endif

    inline SPXACTIVITYHANDLE create_instance()
    {
        SPXACTIVITYHANDLE h;
        SPX_THROW_ON_FAIL(activity_create(&h));
        return h;
    }

#ifndef SWIG
    inline std::string get_prop(const std::string& prop_name) const
    {
        std::size_t size;
        SPX_THROW_ON_FAIL(activity_property_size(m_handle, prop_name.c_str(), &size));
        if (size == 0)
        {
            return std::string{};
        }
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
        SPX_THROW_ON_FAIL(activity_property_get(m_handle, prop_name.c_str(), buffer.get(), size));
        return std::string{ buffer.get(), size };
    }

    inline ro_string_prop build_ro_prop(const std::string& prop_name)
    {
        return ro_string_prop{ [this, prop_name]()
        {
            return get_prop(prop_name);
        } };
    }

    inline string_prop build_prop(const std::string& prop_name)
    {
        return string_prop{
            [this, prop_name]()
            {
                return get_prop(prop_name);
            },
            [this, prop_name](const std::string& v)
            {
                activity_property_set(m_handle, prop_name.c_str(), v.c_str());
            }
        };
    }

    template<typename T>
    T& complex_field(const std::string& field_name, std::map<std::string, T>& registry)
    {
        auto search = registry.find(field_name);
        if (search == registry.end())
        {
            SPXACTIVITYJSONHANDLE jhandle;
            activity_complex_field_handle(m_handle, field_name.c_str(), &jhandle);
            auto r = registry.insert(std::make_pair(field_name, T{ jhandle }));
            return r.first->second;
        }
        return search->second;
    }
#endif
    /*! \endcond */
};

} } } }
