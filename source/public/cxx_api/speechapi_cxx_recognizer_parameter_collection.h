//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer_parameter_collection.h: Public API declarations for RecognizerParameterValueCollection C++ class
//

#pragma once
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_value.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


enum class RecognizerParameter { CustomSpeechModelId = 1 };


class RecognizerParameterValue : public Value
{
public:

    RecognizerParameterValue(SPXRECOHANDLE hreco, const wchar_t* name) : m_hreco(hreco), m_name(name) { }
    RecognizerParameterValue(SPXRECOHANDLE hreco, RecognizerParameter parameter) : m_hreco(hreco), m_name(ParameterNameFromEnum(parameter)) { }

    // --- Value virtual overrides ---

    bool IsString() override { return ContainsString(m_hreco, m_name.c_str()); }
    std::wstring GetString(const wchar_t* defaultValue) override { return GetString(m_hreco, m_name.c_str(), defaultValue); }
    void SetString(const wchar_t* value) override { return SetString(m_hreco, m_name.c_str(), value); }

    bool IsNumber() override { return ContainsNumber(m_hreco, m_name.c_str()); }
    int32_t GetNumber(int32_t defaultValue) override { return GetNumber(m_hreco, m_name.c_str(), defaultValue); }
    void SetNumber(int32_t value) override { SetNumber(m_hreco, m_name.c_str(), value); }

    bool IsBool() override { return ContainsBool(m_hreco, m_name.c_str()); }
    bool GetBool(bool defaultValue) override { return GetBool(m_hreco, m_name.c_str(), defaultValue); }
    void SetBool(bool value) override { SetBool(m_hreco, m_name.c_str(), value); }


private:

    std::wstring ParameterNameFromEnum(RecognizerParameter parameter)
    {
        static_assert((int)RecognizerParameter_CustomSpeechModelId == (int)RecognizerParameter::CustomSpeechModelId, "RecognizerParameter_* enum values == RecognizerParameter::* enum values");

        const size_t maxCharCount = 4096;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Name(static_cast<Recognizer_Parameter>(parameter), sz, maxCharCount));
        return sz;
    }

    static std::wstring GetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* defaultValue)
    {
        const size_t maxCharCount = 1024;
        wchar_t sz[maxCharCount+1];
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_String(hreco, name, sz, maxCharCount, defaultValue));
        return sz;
    }

    static int32_t GetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t defaultValue)
    {
        int32_t value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Int32(hreco, name, &value, defaultValue));
        return value;
    }

    static bool GetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool defaultValue)
    {
        bool value;
        SPX_THROW_ON_FAIL(Recognizer_GetParameter_Bool(hreco, name, &value, defaultValue));
        return !!value;
    }

    static void SetString(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_String(hreco, name, value));
    }

    static void SetNumber(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Int32(hreco, name, value));
    }

    static void SetBool(SPXRECOHANDLE hreco, const wchar_t* name, bool value)
    {
        SPX_THROW_ON_FAIL(Recognizer_SetParameter_Bool(hreco, name, value));
    }

    static bool ContainsString(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_String(hreco, name);
    }

    static bool ContainsNumber(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_Int32(hreco, name);
    }

    static bool ContainsBool(SPXRECOHANDLE hreco, const wchar_t* name)
    {
        return Recognizer_ContainsParameter_Bool(hreco, name);
    }

private:

    SPXRECOHANDLE m_hreco;
    std::wstring m_name;
};


class RecognizerParameterValueCollection : public HandleValueCollection<SPXRECOHANDLE, RecognizerParameterValue>
{
public:

    RecognizerParameterValueCollection(SPXRECOHANDLE hreco) :
        HandleValueCollection(hreco)
    {
    }

    ~RecognizerParameterValueCollection() { SPX_DBG_TRACE_FUNCTION(); }

    Value operator[](const wchar_t* name) override { return Value(new RecognizerParameterValue(m_handle, name)); }
    Value operator[](enum RecognizerParameter parameter) { return Value(new RecognizerParameterValue(m_handle, parameter)); }

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 5 lines of code:
    //
    RecognizerParameterValueCollection() { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterValueCollection(RecognizerParameterValueCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterValueCollection(const RecognizerParameterValueCollection&) : HandleValueCollection(*this) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterValueCollection& operator=(RecognizerParameterValueCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    RecognizerParameterValueCollection& operator=(const RecognizerParameterValueCollection&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 5 lines of code:
    //    
    // RecognizerParameterValueCollection() = delete;
    // RecognizerParameterValueCollection(RecognizerParameterValueCollection&&) = delete;
    // RecognizerParameterValueCollection(const RecognizerParameterValueCollection&) = delete;
    // RecognizerParameterValueCollection& operator=(RecognizerParameterValueCollection&&) = delete;
    // RecognizerParameterValueCollection& operator=(const RecognizerParameterValueCollection&) = delete;
};


} } // CARBON_NAMESPACE_ROOT :: Recognition
