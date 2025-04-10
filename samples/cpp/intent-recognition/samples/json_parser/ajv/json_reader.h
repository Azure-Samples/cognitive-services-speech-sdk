//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_READER_H
#define __AJV_JSON_READER_H

#include <ajv/common.h>
#include <ajv/json_view.h>

namespace ajv {

    class JsonReader
    {
    public:

        JsonReader(const JsonView& view, int item);
        JsonReader(const JsonReader& reader) = default;
        ~JsonReader() = default;

        const JsonView& View(int* item = nullptr) const
        {
            if (item != nullptr) *item = m_item;
            return m_view;
        }

        JsonKind Kind() const { return (JsonKind)m_view.GetKind(m_item); }
        JsonKind Kind(const char** json, size_t* jsize, const char** value, size_t* vsize, bool* boolean, int* integer, double* number) const
        {
            return (JsonKind)m_view.GetKind(m_item, json, jsize, value, vsize, boolean, integer, number);
        }

        bool IsOk() const { auto kind = Kind(); return kind != JsonKind::End && kind != JsonKind::Error && kind != JsonKind::Unspecified; }
        bool IsEnd() const { auto kind = Kind(); return kind == JsonKind::End || kind == JsonKind::Error; }
        bool IsError() const { return Kind() == JsonKind::Error; }
        bool IsEmpty() const { auto kind = Kind(); return kind == JsonKind::End || kind == JsonKind::Error || kind == JsonKind::Unspecified || IsDefault(); }
        bool IsDefault() const;
        bool IsUnspecified() const { return Kind() == JsonKind::Unspecified; }

        bool IsString() const { return Kind() == JsonKind::String; }
        bool IsBool() const { return Kind() == JsonKind::Boolean; }
        bool IsNumber() const { return Kind() == JsonKind::Number; }
        bool IsNull() const { return Kind() == JsonKind::Null; }

        bool IsArray() const { return Kind() == JsonKind::Array; }
        bool IsObject() const { return Kind() == JsonKind::Object; }
        bool IsContainer() const { auto kind = Kind(); return kind == JsonKind::Array || kind == JsonKind::Object; }

        int ValueCount() const { return m_view.Count(m_item); }

        JsonReader ValueAt(int index) const;
        JsonReader ValueAt(const char* name) const;

        JsonReader Name() const;
        JsonReader NameAt(int index) const;
        const char* NamePtrAt(int index, size_t* size) const;

        JsonReader FirstValue() const { return ValueAt(0); }
        JsonReader FirstName() const { return NameAt(0); }
        JsonReader Next() const { return JsonReader(m_view, m_view.Next(m_item)); }
        JsonReader End() const { return JsonReader(m_view, 0); }

        JsonReader operator[](int index) const { return ValueAt(index); }
        JsonReader operator[](const char* name) const { return ValueAt(name); }
        JsonReader operator[](const std::string& name) const { return ValueAt(name.c_str()); }

        JsonReader& operator++() { m_item = m_view.Next(m_item); return *this; }
        JsonReader operator++(int) { auto result = *this; m_item = m_view.Next(m_item); return result; }
        JsonReader& operator=(const JsonReader& rhs) { m_item = rhs.m_item; return *this; }

        bool operator==(const JsonReader& rhs) const { return &m_view == &rhs.m_view && m_item == rhs.m_item; }
        bool operator!=(const JsonReader& rhs) const { return &m_view != &rhs.m_view || m_item != rhs.m_item; }

        std::string AsJson() const;
        const char* AsJsonPtr(size_t* size = nullptr) const;
        
        std::string AsString(const char* defaultValue = "") const;
        std::string AsString(bool decode, const char* defaultValue = "") const;
        const char* AsStringPtr(size_t* size = nullptr) const;

        bool AsBool(bool defaultValue = false) const;
        double AsNumber(double defaultValue = 0) const;
        int64_t AsInt64(int64_t defaultValue = 0) const;
        uint64_t AsUint64(uint64_t defaultValue = 0) const;

        template<class T = int, class = std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value>>
        T AsInt(T defaultValue = 0) const { return (T)(AsInt64(defaultValue)); }
        int AsInt(int defaultValue = 0) const { return AsInt<int>(defaultValue); }

        template<class T = unsigned, class = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
        T AsUint(T defaultValue = 0) const { return (T)(AsUint64(defaultValue)); }

        JsonReader AsArray() const { return IsArray() ? *this : JsonReader(m_view, -1); }
        JsonReader AsObject() const { return IsObject() ? *this : JsonReader(m_view, -1); }

    protected:

        const JsonView& m_view;
        int m_item;
    };

    AJV_FN_NO_INLINE JsonReader::JsonReader(const JsonView& view, int item) : m_view(view), m_item(item)
    {
    }

    AJV_FN_NO_INLINE_(bool) JsonReader::IsDefault() const
    {
        bool boolean;
        double number;
        size_t ssize;
        auto kind = m_view.GetKind(m_item, nullptr, nullptr, nullptr, &ssize, &boolean, nullptr, &number);

        switch (kind)
        {
            case (int)JsonKind::String:
                return ssize == 0;

            case (int)JsonKind::Boolean:
                return boolean == false;

            case (int)JsonKind::Number:
                return number == 0;

            case (int)JsonKind::Array:
            case (int)JsonKind::Object:
                return m_view.Find(m_item, 0, nullptr, nullptr, nullptr) <= 0;

            default:
                return false;
        }
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReader::ValueAt(int index) const
    {
        auto item = m_view.Find(m_item, index, nullptr, nullptr, nullptr);
        return JsonReader(m_view, item);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReader::ValueAt(const char* name) const
    {
        auto item = m_view.Find(m_item, 0, name, nullptr, nullptr);
        return JsonReader(m_view, item);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReader::Name() const
    {
        auto item = m_view.Name(m_item);
        return JsonReader(m_view, item);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReader::NameAt(int index) const
    {
        auto item = IsObject() ? m_view.Find(m_item, index, nullptr, nullptr, nullptr) : -1;
        return JsonReader(m_view, m_view.Name(item));
    }

    AJV_FN_NO_INLINE_(const char*) JsonReader::NamePtrAt(int index, size_t* size) const
    {
        const char* name = nullptr;
        m_view.Find(m_item, index, nullptr, &name, size);
        return name;
    }

    AJV_FN_NO_INLINE_(std::string) JsonReader::AsJson() const
    {
        size_t size = 0;
        auto ptr = AsJsonPtr(&size);
        return ptr != nullptr ? std::string(ptr, size) : "";
    }

    AJV_FN_NO_INLINE_(const char*) JsonReader::AsJsonPtr(size_t* size) const
    {
        const char* ptr = nullptr;
        m_view.GetJson(m_item, &ptr, size);
        return ptr;
    }

    AJV_FN_NO_INLINE_(std::string) JsonReader::AsString(bool decode, const char* defaultValue) const
    {
        size_t size = 0;
        auto ptr = AsStringPtr(&size);
        return ptr != nullptr && decode
            ? JsonString::Encoder::Decode(ptr, size)
            : ptr != nullptr
                ? std::string(ptr, size)
                : defaultValue;
    }

    AJV_FN_NO_INLINE_(std::string) JsonReader::AsString(const char* defaultValue) const
    {
        return AsString(true, defaultValue);
    }

    AJV_FN_NO_INLINE_(const char*) JsonReader::AsStringPtr(size_t* size) const
    {
        const char* ptr = nullptr;
        m_view.GetString(m_item, &ptr, size);
        return ptr;
    }

    AJV_FN_NO_INLINE_(bool) JsonReader::AsBool(bool defaultValue) const
    {
        auto value = false;
        return m_view.GetBool(m_item, &value) ? value : defaultValue;
    }

    AJV_FN_NO_INLINE_(double) JsonReader::AsNumber(double defaultValue) const
    {
        double value = 0;
        return m_view.GetDouble(m_item, &value) ? value : defaultValue;
    }

    AJV_FN_NO_INLINE_(int64_t) JsonReader::AsInt64(int64_t defaultValue) const
    {
        int64_t value = 0;
        return m_view.GetInt64(m_item, &value) ? value : defaultValue;
    }

    AJV_FN_NO_INLINE_(uint64_t) JsonReader::AsUint64(uint64_t defaultValue) const
    {
        uint64_t value = 0;
        return m_view.GetUint64(m_item, &value) ? value : defaultValue;
    }
}

#endif // __AJV_JSON_READER_H
