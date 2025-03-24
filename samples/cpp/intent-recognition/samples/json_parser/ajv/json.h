//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#ifndef __AJV_JSON_H_DEFINED
#define __AJV_JSON_H_DEFINED

namespace ajv
{
    namespace json
    {
        static AJV_FN_NO_INLINE_(JsonReaderView) Parse(std::string& json) { return JsonReaderView(json); }
        static AJV_FN_NO_INLINE_(JsonReaderView) Parse(const char* json, size_t jsize) { return JsonReaderView(json, jsize); }
        
        static AJV_FN_NO_INLINE_(JsonParser) Parse(const std::string& json) { return JsonParser(json.c_str()); }
        static AJV_FN_NO_INLINE_(JsonParser) Parse(const char* psz) { return JsonParser(psz); }

        static AJV_FN_NO_INLINE_(JsonBuilder) Build() { return JsonBuilder(); }
        static AJV_FN_NO_INLINE_(JsonBuilder) Build(const char* psz) { return JsonBuilder::Build(psz); }
        static AJV_FN_NO_INLINE_(JsonBuilder) Build(const std::string& json) { return JsonBuilder::Build(json); }
        static AJV_FN_NO_INLINE_(JsonBuilder) Build(JsonKind kind) { return JsonBuilder(kind); }
    }
}

#endif // __AJV_H_DEFINED
