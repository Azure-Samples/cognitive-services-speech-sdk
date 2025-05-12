//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_BUILDER_H
#define __AJV_JSON_BUILDER_H

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ajv/common.h>
#include <ajv/json_parser.h>

namespace ajv {

    class JsonBuilder : protected JsonParser
    {
    public:

        class JsonWriter : protected JsonReader
        {
        public:

            JsonWriter(JsonBuilder& builder, int item, int parent);
            JsonWriter(const JsonReader& reader, int parent);
            JsonWriter(const JsonWriter& other);
            ~JsonWriter() = default;

            JsonBuilder& Builder() { return m_builder(); }
            const JsonBuilder& Builder() const { return static_cast<const JsonBuilder&>(m_view); }

            JsonBuilder& Builder(int* item) { if (item != nullptr) *item = m_item; return Builder(); }
            const JsonBuilder& Builder(int* item) const { if (item != nullptr) *item = m_item; return Builder(); }

            using JsonReader::Kind;

            using JsonReader::IsOk;
            using JsonReader::IsEnd;
            using JsonReader::IsError;
            using JsonReader::IsEmpty;
            using JsonReader::IsDefault;
            using JsonReader::IsUnspecified;

            using JsonReader::IsString;
            using JsonReader::IsBool;
            using JsonReader::IsNumber;
            using JsonReader::IsNull;

            using JsonReader::IsArray;
            using JsonReader::IsObject;
            using JsonReader::IsContainer;

            using JsonReader::ValueCount;

            JsonWriter ValueAt(int index) const;
            JsonWriter ValueAt(const char* name) const;
            JsonWriter ValueAt(int index, bool create = false);
            JsonWriter ValueAt(const char* name, bool create = false);

            using JsonReader::Name;
            using JsonReader::NameAt;
            using JsonReader::NamePtrAt;

            JsonReader NameEnd() const { return JsonReader(m_view, m_itemEnd); }

            JsonWriter FirstValue() const { return ValueAt(0); }
            using JsonReader::FirstName;

            JsonWriter Next() const { return JsonWriter(m_builder(), m_view.Next(m_item), m_parent); }
            JsonWriter End() const { return JsonWriter(m_builder(), 0, m_item); }

            JsonWriter operator[](int index) const { return ValueAt(index); }
            JsonWriter operator[](const char* name) const { return ValueAt(name); }
            JsonWriter operator[](const std::string& name) const { return ValueAt(name.c_str()); }

            JsonWriter operator[](int index) { return ValueAt(index, true); }
            JsonWriter operator[](const char* name) { return ValueAt(name, true); }
            JsonWriter operator[](const std::string& name) { return ValueAt(name.c_str(), true); }

            JsonWriter& operator++() { m_item = m_view.Next(m_item); return *this; }
            JsonWriter operator++(int) { auto result = *this; m_item = m_view.Next(m_item); return result; }
            JsonWriter& operator=(const JsonWriter& rhs)
            {
                m_parent = rhs.m_parent;
                m_item = rhs.m_item;
                return *this;
            }

            bool operator==(const JsonWriter& rhs) const { return &m_view == &rhs.m_view && m_parent == rhs.m_parent && m_item == rhs.m_item; }
            bool operator!=(const JsonWriter& rhs) const { return &m_view != &rhs.m_view || m_parent != rhs.m_parent || m_item != rhs.m_item; }

            bool operator==(const std::string& rhs) const { return AsString() == rhs; }

            JsonWriter& operator|=(const JsonBuilder& other);

            template<class T>
            JsonWriter& operator^=(T value) { return !IsEmpty() ? *this : *this <<= value; }
            JsonWriter& operator^=(const std::vector<std::string>& items) { return !IsEmpty() ? *this : *this <<= items; }
            JsonWriter& operator^=(const std::map<std::string, std::string>& items) { return !IsEmpty() ? *this : *this <<= items; }

            template<class T>
            JsonWriter& operator<<=(T value) { return value == 0 ? *this : *this = value; }
            JsonWriter& operator<<=(const std::vector<std::string>& items) { return items.size() == 0 ? *this : *this = items; }
            JsonWriter& operator<<=(const std::map<std::string, std::string>& items) { return items.size() == 0 ? *this : *this = items; }
            JsonWriter& operator<<=(const std::string& value) { return value.length() == 0 ? *this : *this = value.c_str(); }
            JsonWriter& operator<<=(const char* value) { return value == nullptr || value[0] == '\0' ? *this : *this = value; }

            JsonWriter& operator=(const JsonReaderView& view) { return Parse(view.AsJson()); }
            JsonWriter& operator=(const JsonBuilder& builder) { return Parse(builder.AsJson()); }
            JsonWriter& operator=(const JsonParser& parser) { return Parse(parser.AsJson()); }
            JsonWriter& operator=(const JsonReader& reader) { return Parse(reader.AsJson()); }
            JsonWriter& Parse(const std::string& str);

            template<class T>
            JsonWriter& operator=(T value) { return SetValue<T>(value); }
            JsonWriter& operator=(const std::vector<std::string>& items);
            JsonWriter& operator=(const std::map<std::string, std::string>& items);
            inline JsonWriter& operator=(const std::string& value) { *this = value.c_str(); return *this; }
            JsonWriter& operator=(const char* value);
            inline JsonWriter& operator=(bool value) { return SetValue(value); }
            inline JsonWriter& operator=(std::nullptr_t) { return SetValue(nullptr); }

            std::string AsJson() const;

            using JsonReader::AsString;
            using JsonReader::AsStringPtr;

            using JsonReader::AsBool;

            using JsonReader::AsNumber;
            using JsonReader::AsInt;
            using JsonReader::AsUint;
            using JsonReader::AsInt64;
            using JsonReader::AsUint64;

            JsonWriter AsArray() const { return IsArray() ? *this : JsonWriter(m_builder(), -1, m_item); }
            JsonWriter AsObject() const { return IsObject() ? *this : JsonWriter(m_builder(), -1, m_item); }

        protected:

            JsonBuilder& m_builder() const { return const_cast<JsonBuilder&>(static_cast<const JsonBuilder&>(m_view)); }

            int AddItem(int index);
            int AddItem(const char* name);

            int InitItemNull();
            int InitItemUnspecified() { return m_builder().InitItemUnspecified(); }

            const char* DupEncodeQuoted(const char* add, size_t cch, size_t* cchUpdated);

            JsonWriter& SetValue(const std::string& value);
            JsonWriter& SetValue(const char* value, size_t cch);

            JsonWriter& SetValue(bool value);

            template<class T>
            JsonBuilder::JsonWriter& SetValue(T value);

            JsonBuilder::JsonWriter& SetValue(std::nullptr_t);

            int m_parent;

            static constexpr const char* m_nullValue = "null";
            static constexpr size_t m_cchNullValue = 4;
        };

        JsonBuilder();
        JsonBuilder(JsonKind kind);
        JsonBuilder(const char* psz);
        JsonBuilder(const std::string& json) : JsonBuilder(json.c_str()) {}
        ~JsonBuilder();

        JsonBuilder(const JsonBuilder& other);
        JsonBuilder& operator=(const JsonBuilder& other);

        JsonBuilder(JsonBuilder&& other);
        JsonBuilder& operator=(JsonBuilder&& other);

        JsonWriter Writer() { return m_root; }
        JsonWriter Writer(int item) { return JsonWriter(*this, item, Parent(item)); }

        static JsonBuilder Build(const char* psz) { return JsonBuilder(psz); }
        static JsonBuilder Build(const std::string& str) { return JsonBuilder(str); }

        JsonKind Kind() const { return m_root.Kind(); }

        bool IsOk() const { return m_root.IsOk(); }
        bool IsEnd() const { return m_root.IsEnd(); }
        bool IsError() const { return m_root.IsError(); }
        bool IsEmpty() const { return m_root.IsEmpty(); }
        bool IsDefault() const { return m_root.IsDefault(); }
        bool IsUnspecified() const { return m_root.IsUnspecified(); }

        bool IsString() const { return m_root.IsString(); }
        bool IsBool() const { return m_root.IsBool(); }
        bool IsNumber() const { return m_root.IsNumber(); }
        bool IsNull() const { return m_root.IsNull(); }

        bool IsArray() const { return m_root.IsArray(); }
        bool IsObject() const { return m_root.IsObject(); }
        bool IsContainer() const { return m_root.IsContainer(); }

        int ValueCount() const { return m_root.ValueCount(); }

        JsonWriter ValueAt(int index) const { return m_root.ValueAt(index); }
        JsonWriter ValueAt(const char* name) const { return m_root.ValueAt(name); }
        JsonWriter ValueAt(int index, bool create = false) { return m_root.ValueAt(index, create); }
        JsonWriter ValueAt(const char* name, bool create = false) { return m_root.ValueAt(name, create); }

        JsonReader NameAt(int index) const { return m_root.NameAt(index); }
        const char* NamePtrAt(int index, size_t* size) const { return m_root.NamePtrAt(index, size); }
        JsonReader NameEnd() const { return m_root.NameEnd(); }

        JsonWriter FirstValue() const { return m_root.FirstValue(); }
        JsonReader FirstName() const { return m_root.FirstName(); }

        JsonWriter End() const { return m_root.End(); }

        JsonWriter operator[](int index) const { return ValueAt(index); }
        JsonWriter operator[](const char* name) const { return ValueAt(name); }
        JsonWriter operator[](const std::string& name) const { return ValueAt(name.c_str()); }

        JsonWriter operator[](int index) { return ValueAt(index, true); }
        JsonWriter operator[](const char* name) { return ValueAt(name, true); }
        JsonWriter operator[](const std::string& name) { return ValueAt(name.c_str(), true); }

        JsonBuilder& operator|=(const JsonBuilder& other) { m_root |= other; return *this; }

        template<class T> JsonBuilder& operator^=(T&& value) { m_root ^= value; return *this; }
        template<class T> JsonBuilder& operator<<=(T&& value) { m_root <<= value; return *this; }
        template<class T> JsonBuilder& operator=(T&& value) { m_root = value; return *this; }
        JsonBuilder& operator=(std::nullptr_t) { m_root = nullptr; return *this; }

        std::string AsJson() const { return m_root.AsJson(); }

        std::string AsString(bool decode, const char* defaultValue = "") const { return m_root.AsString(decode, defaultValue); }
        std::string AsString(const char* defaultValue = "") const { return m_root.AsString(defaultValue); }
        const char* AsStringPtr(size_t* size = nullptr) const { return m_root.AsStringPtr(size); }

        bool AsBool(bool defaultValue = false) const { return m_root.AsBool(defaultValue); }

        double AsNumber(double defaultValue = 0) const { return m_root.AsNumber(defaultValue); }
        int64_t AsInt64(int64_t defaultValue = 0) const { return m_root.AsInt64(defaultValue); }
        uint64_t AsUint64(uint64_t defaultValue = 0) const { return m_root.AsUint64(defaultValue); }

        template<class T = int, class = std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value>>
        T AsInt(T defaultValue = 0) const { return m_root.AsInt<T>(defaultValue); }
        int AsInt(int defaultValue = 0) const { return m_root.AsInt(defaultValue); }

        template<class T = unsigned, class = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
        T AsUint(T defaultValue = 0) const { return m_root.AsUint<T>(defaultValue); }

        JsonWriter AsArray() const { return m_root.AsArray(); }
        JsonWriter AsObject() const { return m_root.AsObject(); }

    protected:

        int GetJson(int item, char* psz, char* zend) const;
        int GetJsonArray(int item, char* psz, char* zend) const;
        int GetJsonObject(int item, char* psz, char* zend) const;
        int GetJsonMember(int item, char* psz, char* zend) const;
        int GetJsonItem(int item, char* psz, char* zend) const;

        JsonWriter m_root;
    };

    AJV_FN_NO_INLINE JsonBuilder::JsonWriter::JsonWriter(const JsonReader& reader, int parent) : JsonReader(reader), m_parent(parent)
    {
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonWriter::JsonWriter(JsonBuilder& builder, int item, int parent) : JsonReader(builder, item), m_parent(parent)
    {
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonWriter::JsonWriter(const JsonWriter& other) : JsonReader(other), m_parent(other.m_parent)
    {
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter) JsonBuilder::JsonWriter::ValueAt(int index) const
    {
        auto item = m_view.Find(m_item, index, nullptr, nullptr, nullptr);
        return JsonWriter(m_builder(), item, m_item);
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter) JsonBuilder::JsonWriter::ValueAt(const char* name) const
    {
        auto item = m_view.Find(m_item, 0, name, nullptr, nullptr);
        return JsonWriter(m_builder(), item, m_item);
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter) JsonBuilder::JsonWriter::ValueAt(int index, bool create)
    {
        auto item = m_view.Find(m_item, index, nullptr, nullptr, nullptr);
        if (item <= 0 && create) item = AddItem(index);
        return JsonWriter(m_builder(), item, m_item);
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter) JsonBuilder::JsonWriter::ValueAt(const char* name, bool create)
    {
        auto item = m_view.Find(m_item, 0, name, nullptr, nullptr);
        if (item <= 0 && create) item = AddItem(name);
        return JsonWriter(m_builder(), item, m_item);
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::Parse(const std::string& str)
    {
        if (m_item <= m_itemEnd) { m_item = -1; return *this; }

        auto& builder = m_builder();

        // make a copy of the json string
        auto psz = builder.DupPsz(str.c_str());
        auto cch = str.length();

        // re-parse the json, into this builder
        auto iadded = builder.PeekItem();
        auto zend = psz + cch;
        auto check = builder.ParseElement(psz, zend);

        // deal with parse errors...
        auto& item = builder.m_items[m_item];
        if (check > zend)
        {
            item.start = item.end = nullptr;
            item.first = m_itemEnd;
            return *this;
        }

        // set start, end, and first in normal cases ...
        auto& added = builder.m_items[iadded];
        auto kind = added.start[0];
        if (IsStartArray(kind) || IsStartObject(kind))
        {
            item.start = item.end = added.start;
            item.first = added.first;
        }
        else
        {
            item.start = added.start;
            item.end = added.end;
            item.first = m_itemEnd;
        }

        return *this;
    }

    AJV_FN_NO_INLINE_(std::string) JsonBuilder::JsonWriter::AsJson() const
    {
        auto& builder = m_builder();

        constexpr int maxStackBufferSize = 4096;
        char buffer[maxStackBufferSize + 1];

        auto psz = buffer;
        auto zend = psz + maxStackBufferSize;

        auto size = builder.GetJson(m_item, psz, zend);
        if (size >= maxStackBufferSize)
        {
            psz = new char[size + 1];
            zend = psz + size;
            size = builder.GetJson(m_item, psz, zend);
        }

        auto json = std::string(psz, size);
        if (psz != buffer) delete[] psz;

        return json;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::operator=(const std::vector<std::string>& items)
    {
        int i = 0;
        for (auto& item : items)
        {
            (*this)[i++] = item;
        }
        return !IsArray() ? Parse("[]") : *this;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::operator=(const std::map<std::string, std::string>& items)
    {
        for (auto& item : items)
        {
            auto& name = item.first;
            auto& value = item.second;
            (*this)[name] = value;
        }
        return !IsObject() ? Parse("{}") : *this;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::operator=(const char* value)
    {
        // add the value as quoted encoded string
        auto cch = strlen(value);
        auto psz = DupEncodeQuoted(value, cch, &cch);

        // set start and end
        return SetValue(psz, cch);
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::JsonWriter::AddItem(int index)
    {
        if (m_item <= m_itemEnd) return -1;

        auto& builder = m_builder();

        // parent isn't an array yet, make it an array
        if (Kind() != JsonKind::Array)
        {
            auto& parent = builder.m_items[m_item];
            parent.start = parent.end = "[";
            parent.first = m_itemEnd;
        }

        // loop thru ittems, finding the item requested
        auto ilast = 0;
        auto ivalue = 0;
        for (int i = 0; i <= index; i++)
        {
            ilast = ivalue; // keep track of the last value we checked/added
            ivalue = m_view.Find(m_item, i, nullptr, nullptr, nullptr);
            if (ivalue > 0) continue; // array item at i already exists

            // init the value as null for preceeding array elements, and unspecified for this added item
            ivalue = i < index ? InitItemNull() : InitItemUnspecified();

            // now, depending on the item added's position
            if (i == 0)
            {
                // didn't have value yet... hook up the new added item as "first"
                builder.m_items[m_item].first = ivalue;
            }
            else
            {
                // set the last child's value's next
                auto plastValue = &builder.m_items[ilast];
                plastValue->next = ivalue;
            }
        }

        return ivalue;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::JsonWriter::AddItem(const char* name)
    {
        if (m_item <= m_itemEnd) return -1;

        auto& builder = m_builder();

        // add the name as quoted string
        auto cch = strlen(name);
        auto psz = DupEncodeQuoted(name, cch, &cch);
        auto iname = builder.InitItem(psz);
        builder.EndItem(iname, psz + cch - 1);

        // init the value as unspecified and hook to name
        auto ivalue = InitItemUnspecified();
        auto pname = &builder.m_items[iname];
        pname->namesValue = ivalue;

        // now, depending on the added item's parent (m_item) ...
        auto& item = builder.m_items[m_item];
        if (Kind() != JsonKind::Object)
        {
            // item wasn't an object yet, make it an object, and hook up the new added item
            item.start = item.end = "{";
            item.first = iname;
        }
        else if (item.first == 0)
        {
            // it was an object, but it didn't have any values yet... hook up the new added item as "first"
            item.first = iname;
        }
        else
        {
            // it was an object, and it had children already, so, let's find the last child
            auto ilast = item.first;
            for (auto i = ilast; i > 0; i = builder.m_items[i].next)
            {
                ilast = i;
            }

            // set the last child's name's next
            auto pnameLast = &builder.m_items[ilast];
            pnameLast->next = iname;

            // set the last child's value's next
            auto ivalueLast = pnameLast->namesValue;
            auto pvalueLast = &builder.m_items[ivalueLast];
            pvalueLast->next = ivalue;
        }

        return ivalue;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::JsonWriter::InitItemNull()
    {
        auto& builder = m_builder();
        auto item = builder.InitItem(m_nullValue);
        builder.EndItem(item, m_nullValue + m_cchNullValue - 1);
        return item;
    }

    AJV_FN_NO_INLINE_(const char*) JsonBuilder::JsonWriter::DupEncodeQuoted(const char* add, size_t cch, size_t* cchUpdated)
    {
        auto encodedSize = JsonString::Encoder::EncodedSize(add, cch);
        auto quotedSize = 2 + encodedSize + 1;

        auto deleter = [](char* p) { delete[] p; };
        std::unique_ptr<char[], decltype(deleter)> quoted(new char[quotedSize], deleter);

        quoted[0] = '\"';
        size_t cchQuoted = 1;

        char* dest = quoted.get() + 1;
        size_t cchEncoded = JsonString::Encoder::Encode(add, cch, dest, encodedSize);
        cchQuoted += cchEncoded;

        quoted[cchQuoted++] = '\"';
        quoted[cchQuoted] = '\0';
        *cchUpdated = cchQuoted;

        return m_builder().DupPsz(quoted.get());
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::SetValue(const std::string& value)
    {
        auto& builder = m_builder();
        auto psz = builder.DupPsz(value.c_str());
        SetValue(psz, value.length());
        return *this;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::SetValue(const char* value, size_t cch)
    {
        if (m_item <= m_itemEnd) { m_item = -1; return *this; }

        // set start and end
        auto& builder = m_builder();
        auto& item = builder.m_items[m_item];
        item.start = value;
        item.end = value + cch - 1;

        // values have no first
        item.first = m_itemEnd;

        return *this;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::SetValue(bool value)
    {
        const char* psz = value ? "true" : "false";
        auto cch = value ? 4 : 5;
        return SetValue(psz, cch);
    }

    template<class T>
    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::SetValue(T value)
    {
        static_assert(std::is_arithmetic<T>::value, "T must be integral");
        return SetValue(std::to_string(value));
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::SetValue(std::nullptr_t)
    {
        return SetValue(m_nullValue, m_cchNullValue);
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonBuilder() : JsonParser(), m_root(m_readerRoot, -1)
    {
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonBuilder(JsonKind kind) : JsonBuilder()
    {
        m_root = JsonParser::Parse(
            kind == JsonKind::String ? "\"\""
            : kind == JsonKind::Boolean ? "false"
            : kind == JsonKind::Number ? "0"
            : kind == JsonKind::Null ? "null"
            : kind == JsonKind::Array ? "[]"
            : kind == JsonKind::Object ? "{}"
            : "");
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonBuilder(const char* psz) : JsonParser(psz), m_root(m_readerRoot, -1)
    {
    }

    AJV_FN_NO_INLINE JsonBuilder::~JsonBuilder()
    {
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonBuilder(const JsonBuilder& other) : JsonBuilder(other.AsJson().c_str())
    {
    }

    AJV_FN_NO_INLINE JsonBuilder& JsonBuilder::operator=(const JsonBuilder& other)
    {
        JsonParser::operator=(other);
        m_root = JsonWriter(m_readerRoot, -1);
        return *this;
    }

    AJV_FN_NO_INLINE JsonBuilder::JsonBuilder(JsonBuilder&& other) : JsonParser(std::move(other)), m_root(m_readerRoot, -1)
    {
    }

    AJV_FN_NO_INLINE JsonBuilder& JsonBuilder::operator=(JsonBuilder&& other)
    {
        JsonParser::operator=(std::move(other));
        m_root = JsonWriter(m_readerRoot, -1);
        return *this;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::GetJson(int item, char* psz, char* zend) const
    {
        auto kind = (JsonKind)GetKind(item);
        if (kind == JsonKind::Array) return GetJsonArray(item, psz, zend);
        if (kind == JsonKind::Object) return GetJsonObject(item, psz, zend);
        if (kind == JsonKind::String) return GetJsonItem(item, psz, zend);
        if (kind == JsonKind::Boolean) return GetJsonItem(item, psz, zend);
        if (kind == JsonKind::Number) return GetJsonItem(item, psz, zend);
        if (kind == JsonKind::Null) return GetJsonItem(item, psz, zend);
        return 0;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::GetJsonArray(int item, char* psz, char* zend) const
    {
        auto& parent = m_items[item];
        auto parentSize = 0;

        // add opening char for array container
        if (psz < zend) *psz++ = '[';
        parentSize++;

        // add all array elements (with non empty values), starting with the first
        auto nonEmpty = 0;
        auto ivalue = parent.firstInArray;
        while (ivalue > 0)
        {
            // comma separated, after first element
            auto addComma = nonEmpty > 0;
            if (addComma)
            {
                if (psz < zend) *psz++ = ',';
                parentSize++;
            }

            // add the first/next element
            auto elemSize = GetJson(ivalue, psz, zend);
            if (elemSize > 0)
            {
                parentSize += elemSize;
                psz += elemSize;
                nonEmpty++;
            }
            else if (addComma) // empty element, backup over comma
            {
                if (psz <= zend) psz--;
                parentSize--;
            }

            ivalue = m_items[ivalue].next;
        }

        // add closing char for array container, and terminate
        parentSize++;
        if (psz < zend)
        {
            *psz++ = ']';
            if (psz < zend)
            {
                *psz = '\0';
            }
        }

        return parentSize;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::GetJsonObject(int item, char* psz, char* zend) const
    {
        auto& parent = m_items[item];
        auto parentSize = 0;

        // add opening char for object container
        if (psz < zend) *psz++ = '{';
        parentSize++;

        // add all object members (with non empty values), starting with the first
        auto nonEmpty = 0;
        auto ivalue = parent.firstInObject;
        while (ivalue > 0)
        {
            // comma separated, after first member
            auto addComma = nonEmpty > 0;
            if (addComma)
            {
                if (psz < zend) *psz++ = ',';
                parentSize++;
            }

            // add the first/next member
            auto memberSize = GetJsonMember(ivalue, psz, zend);
            if (memberSize > 0)
            {
                parentSize += memberSize;
                psz += memberSize;
                nonEmpty++;
            }
            else if (addComma) // empty member, backup over comma
            {
                if (psz <= zend) psz--;
                parentSize--;
            }

            ivalue = m_items[ivalue].next;
        }

        // add closing char for object container, and terminate
        parentSize++;
        if (psz < zend)
        {
            *psz++ = '}';
            if (psz < zend)
            {
                *psz = '\0';
            }
        }

        return parentSize;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::GetJsonMember(int iname, char* psz, char* zend) const
    {
        auto memberSize = 0;

        // add the name
        auto nameSize = GetJsonItem(iname, psz, zend);
        memberSize += nameSize;
        psz += nameSize;

        // add the colon
        if (psz < zend) *psz++ = ':';
        memberSize++; // colon size=1

        // add the value
        auto ivalue = m_items[iname].namesValue;
        auto valueSize = GetJson(ivalue, psz, zend);
        memberSize += valueSize;
        psz += valueSize;

        // deal with possibly empty value member
        auto emptyValue = valueSize == 0;
        if (emptyValue)
        {
            psz -= memberSize;
            memberSize = 0;
        }

        if (psz < zend) *psz = '\0';

        return memberSize;
    }

    AJV_FN_NO_INLINE_(int) JsonBuilder::GetJsonItem(int ivalue, char* psz, char* zend) const
    {
        auto &item = m_items[ivalue];
        auto pitem = item.start;
        auto zitem = item.end + 1;
        auto itemSize = zitem - pitem;

        while (psz < zend && pitem < zitem) *psz++ = *pitem++;
        if (psz < zend) *psz = '\0';

        return (int)itemSize;
    }

    AJV_FN_NO_INLINE_(JsonBuilder::JsonWriter&) JsonBuilder::JsonWriter::operator|=(const JsonBuilder& other)
    {
        if ((IsObject() || IsUnspecified()) && other.IsObject())
        {
            for (auto i = 0; i < other.ValueCount(); i++)
            {
                auto key = other.NameAt(i).AsString();
                auto value = other[key].AsJson();
                JsonParser parser{ value.c_str() };
                (*this)[key] = parser;
            }
        }
        else if ((IsArray() || IsUnspecified()) && other.IsArray())
        {
            auto size = ValueCount();
            for (auto i = 0; i < other.ValueCount(); i++)
            {
                auto value = other[i].AsJson();
                JsonParser parser{ value.c_str() };
                (*this)[size + i] = parser;
            }
        }
        return *this;
    }

}

#endif // __AJV_JSON_BUILDER_H
