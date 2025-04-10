//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_PARSER_H
#define __AJV_JSON_PARSER_H

#include <ajv/common.h>
#include <ajv/json_reader_view.h>

namespace ajv {

    class JsonParser : protected JsonView
    {
    public:

        JsonParser();
        JsonParser(const char* psz);
        JsonParser(const std::string& json) : JsonParser(json.c_str()) {}
        ~JsonParser();

        JsonParser(const JsonParser& other);
        JsonParser& operator=(const JsonParser& other);

        JsonParser(JsonParser&& other);
        JsonParser& operator=(JsonParser&& other);

        const JsonView& View(int* item = nullptr) const { return m_readerRoot.View(item); }

        JsonReader Reader() const { return JsonReader(m_readerRoot); }
        JsonReader Reader(int item) const { return JsonReader(*this, item); }

        static JsonParser Parse(const char* psz) { return JsonParser(psz); }
        static JsonParser Parse(const std::string& json) { return JsonParser(json); }

        static JsonReaderView Parse(const char* json, size_t jsize) { return JsonReaderView(json, jsize); }

        JsonKind Kind() const { return m_readerRoot.Kind(); }

        bool IsOk() const { return m_readerRoot.IsOk(); }
        bool IsEnd() const { return m_readerRoot.IsEnd(); }
        bool IsError() const { return m_readerRoot.IsError(); }
        bool IsEmpty() const { return m_readerRoot.IsEmpty(); }
        bool IsDefault() const { return m_readerRoot.IsDefault(); }
        bool IsUnspecified() const { return m_readerRoot.IsUnspecified(); }

        bool IsString() const { return m_readerRoot.IsString(); }
        bool IsBool() const { return m_readerRoot.IsBool(); }
        bool IsNumber() const { return m_readerRoot.IsNumber(); }
        bool IsNull() const { return m_readerRoot.IsNull(); }

        bool IsArray() const { return m_readerRoot.IsArray(); }
        bool IsObject() const { return m_readerRoot.IsObject(); }
        bool IsContainer() const { return m_readerRoot.IsContainer(); }

        int ValueCount() const { return m_readerRoot.ValueCount(); }

        JsonReader ValueAt(int index) const { return m_readerRoot.ValueAt(index); }
        JsonReader ValueAt(const char* name) const { return m_readerRoot.ValueAt(name); }

        JsonReader NameAt(int index) const { return m_readerRoot.NameAt(index); }
        const char* NamePtrAt(int index, size_t* size) const { return m_readerRoot.NamePtrAt(index, size); }

        JsonReader FirstValue() const { return m_readerRoot.FirstValue(); }
        JsonReader FirstName() const { return m_readerRoot.FirstName(); }
        JsonReader End() const { return m_readerRoot.End(); }

        JsonReader operator[](int index) const { return ValueAt(index); }
        JsonReader operator[](const char* name) const { return ValueAt(name); }
        JsonReader operator[](const std::string& name) const { return ValueAt(name.c_str()); }

        std::string AsJson() const { return m_readerRoot.AsJson(); }

        std::string AsString(const char* defaultValue = "") const { return m_readerRoot.AsString(defaultValue); }
        const char* AsStringPtr(size_t* size = nullptr) const { return m_readerRoot.AsStringPtr(size); }

        bool AsBool(bool defaultValue = false) const { return m_readerRoot.AsBool(defaultValue); }
        double AsNumber(double defaultValue = 0) const { return m_readerRoot.AsNumber(defaultValue); }
        int64_t AsInt64(int64_t defaultValue = 0) const { return m_readerRoot.AsInt64(defaultValue); }
        uint64_t AsUint64(uint64_t defaultValue = 0) const { return m_readerRoot.AsUint64(defaultValue); }

        template<class T = int, class = std::enable_if_t<std::is_integral<T>::value && !std::is_unsigned<T>::value>>
        T AsInt(T defaultValue = 0) const { return m_readerRoot.AsInt<T>(defaultValue); }
        int AsInt(int defaultValue = 0) const { return m_readerRoot.AsInt(defaultValue); }

        template<class T = unsigned, class = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
        T AsUint(T defaultValue = 0) const { return m_readerRoot.AsUint<T>(defaultValue); }

        JsonReader AsArray() const { return m_readerRoot.AsArray(); }
        JsonReader AsObject() const { return m_readerRoot.AsObject(); }

    protected:

        JsonReader InitRoot();
        JsonReader InitRoot(const char* psz);
        JsonReader InitMoveRoot(JsonParser& parser, const JsonParser& other);

        JsonReader ParseDupPsz(const char* psz);
        JsonReader ParsePsz(const char* psz);
        JsonReader ParsePtr(const char* ptr, size_t cch);

        const char* DupPsz(const char* psz);
        const char* DupPtr(const char* ptr, size_t cch);
        void FreeDups();

        static constexpr int m_itemAllocRoot{ 0 };
        int m_itemAlloc{ 0 };

        JsonReader m_readerRoot;
    };

    AJV_FN_NO_INLINE JsonParser::JsonParser() : m_itemAlloc(0), m_readerRoot(InitRoot())
    {
    }

    AJV_FN_NO_INLINE JsonParser::JsonParser(const char* psz) : m_itemAlloc(0), m_readerRoot(InitRoot(psz))
    {
    }

    AJV_FN_NO_INLINE JsonParser::~JsonParser()
    {
        FreeDups();
    }

     AJV_FN_NO_INLINE JsonParser::JsonParser(const JsonParser& other) : JsonParser(other.AsJson())
     {
     }

     AJV_FN_NO_INLINE_(JsonParser&) JsonParser::operator=(const JsonParser& other)
     {
         FreeDups();

         m_itemAlloc = 0;
         m_items.resize(m_itemCountResize);

         m_readerRoot = InitRoot(other.AsJson().c_str());
         return *this;
     }

    AJV_FN_NO_INLINE JsonParser::JsonParser(JsonParser&& other) :
        JsonView(std::move(other)),
        m_readerRoot(InitMoveRoot(*this, other))
    {
    }

    AJV_FN_NO_INLINE_(JsonParser&) JsonParser::operator=(JsonParser&& other)
    {
        FreeDups();

        JsonView::operator=(std::move(other));

        m_itemAlloc = other.m_itemAlloc;
        m_readerRoot = InitMoveRoot(*this, other);
        return *this;
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::InitRoot()
    {
        auto root = InitItemUnspecified();
        return JsonReader(*this, root);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::InitRoot(const char* psz)
    {
        return IsNullOrWhiteSpace(psz) ? InitRoot() : ParseDupPsz(psz);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::InitMoveRoot(JsonParser& parser, const JsonParser& other)
    {
        int item;
        other.View(&item);
        return JsonReader(parser, item);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::ParseDupPsz(const char* psz)
    {
        return ParsePsz(DupPsz(psz));
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::ParsePsz(const char* psz)
    {
        auto cch = psz != nullptr ? strlen(psz) : 0;
        return ParsePtr(psz, cch);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonParser::ParsePtr(const char* ptr, size_t cch)
    {
        auto item = JsonView::Parse(ptr, cch);
        return JsonReader(*this, item > 0 ? item : -1);
    }

    AJV_FN_NO_INLINE_(const char*) JsonParser::DupPsz(const char* psz)
    {
        auto cch = psz != nullptr ? strlen(psz) : 0;
        return DupPtr(psz, cch);
    }

    AJV_FN_NO_INLINE_(const char*) JsonParser::DupPtr(const char* ptr, size_t cch)
    {
        auto next = InitItem(nullptr);
        EndItem(next, nullptr);
        m_items[m_itemAlloc].next = next;

        char *copy = new char[cch + 1];
        m_items[m_itemAlloc].start = copy;
        m_itemAlloc = next;

        auto end = ptr + cch;
        for (auto dest = copy; ptr < end && *ptr != '\0'; dest++, ptr++) *dest = *ptr;
        copy[cch] = '\0';

        return copy;
    }

    AJV_FN_NO_INLINE_(void) JsonParser::FreeDups()
    {
        if (m_itemCount > 0)
        {
            auto i = m_itemAllocRoot;
            while (true)
            {
                auto psz = m_items[i].start;
                if (psz != nullptr) delete [] psz;
                i = m_items[i].next;
                if (i <= 0) break;
            }
        }
    }
}

#endif // __AJV_JSON_PARSER_H
