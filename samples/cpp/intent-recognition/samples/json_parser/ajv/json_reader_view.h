//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_READER_VIEW_H
#define __AJV_JSON_READER_VIEW_H

#include <ajv/json_reader.h>
#include <ajv/json_view.h>

namespace ajv {

    class JsonReaderView : protected JsonView
    {
    public:

        JsonReaderView(const char* psz);
        JsonReaderView(const char* json, size_t jsize);
        JsonReaderView(std::string& json);
        ~JsonReaderView() = default;

        const JsonView& View(int* item = nullptr) const { return m_readerRoot.View(item); }

        JsonReader Reader() const { return m_readerRoot; }
        JsonReader Reader(int item) const { return JsonReader(*this, item); }

        static JsonReaderView Parse(const char* json, size_t jsize) { return JsonReaderView(json, jsize); }
        static JsonReaderView Parse(const char* psz) { return JsonReaderView(psz); }
        static JsonReaderView Parse(std::string& json) { return JsonReaderView(json); }

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

        JsonReader m_readerRoot;

        JsonReader InitRoot(const char* json, size_t jsize);
        JsonReader ParsePtr(const char* json, size_t jsize);

    };

    AJV_FN_NO_INLINE JsonReaderView::JsonReaderView(const char* psz) : JsonReaderView(psz, strlen(psz))
    {
    }

    AJV_FN_NO_INLINE JsonReaderView::JsonReaderView(const char* json, size_t jsize) : m_readerRoot(InitRoot(json, jsize))
    {
    }

    AJV_FN_NO_INLINE JsonReaderView::JsonReaderView(std::string& json) : JsonReaderView(json.c_str(), json.size())
    {
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReaderView::InitRoot(const char* json, size_t jsize)
    {
        return IsNullOrWhiteSpace(json, jsize)
            ? JsonReader(*this, 0)
            : ParsePtr(json, jsize);
    }

    AJV_FN_NO_INLINE_(JsonReader) JsonReaderView::ParsePtr(const char* json, size_t jsize)
    {
        auto item = JsonView::Parse(json, jsize);
        return JsonReader(*this, item > 0 ? item : -1);
    }

}

#endif // __AJV_JSON_READER_VIEW_H
