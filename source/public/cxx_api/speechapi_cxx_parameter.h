//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_parameter.h: Public API declarations for Parameter C++ class
//

#pragma once
#include <functional>
#include <speechapi_c_common.h>


namespace CARBON_NAMESPACE_ROOT {


class Parameter
{
public:

    Parameter(Parameter *ptr = nullptr) : m_delegatePtr(ptr) { }
    Parameter(Parameter&& other) : m_delegatePtr(std::move(other.m_delegatePtr)) { }

    virtual ~Parameter()
    {
        if (m_delegatePtr != nullptr)
        {
            delete m_delegatePtr;
        }
    }

    virtual bool HasString() { return m_delegatePtr->HasString(); }
    virtual void SetString(const wchar_t* value) { m_delegatePtr->SetString(value); }
    virtual std::wstring GetString(const wchar_t* defaultValue = L"") { return m_delegatePtr->GetString(defaultValue); }

    virtual bool HasNumber() { return m_delegatePtr->HasNumber(); }
    virtual void SetNumber(int32_t value) { m_delegatePtr->SetNumber(value); }
    virtual int32_t GetNumber(int32_t defaultValue = 0) { return m_delegatePtr->GetNumber(defaultValue); }

    virtual bool HasBool() { return m_delegatePtr->HasBool(); }
    virtual void SetBool(bool value) { m_delegatePtr->SetBool(value); }
    virtual bool GetBool(bool defaultValue = false) { return m_delegatePtr->GetBool(defaultValue); }

    const wchar_t* operator=(const wchar_t* value) { SetString(value); return value; }
    int32_t operator=(int32_t value) { SetNumber(value); return value; }
    bool operator=(bool value) { SetBool(value); return value; }

    operator const std::wstring() { return GetString(); }
    operator int32_t() { return GetNumber(); }
    operator bool() { return GetBool(); }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 3 lines of code:
    //
    Parameter(const Parameter&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    Parameter& operator=(Parameter&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    const Parameter& operator=(const Parameter&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 3 lines of code:
    //
    // Parameter(const Parameter&) = delete;
    // Parameter& operator=(Parameter&&) = delete;
    // const Parameter& operator=(const Parameter&) = delete;

    Parameter* m_delegatePtr;
};


template<class Handle, class Param>
class ParameterCollection
{
public:

    ParameterCollection(Handle handle) : m_handle(handle) { }

    Parameter operator[](const wchar_t* name) { return Parameter(new Param(m_handle, name)); }

    bool HasString(const wchar_t* name) { return Param(m_handle, name).HasString(); }
    void SetString(const wchar_t* name, const wchar_t* value) { Param(m_handle, name).SetString(value); }
    std::wstring GetString(const wchar_t* name, const wchar_t* defaultValue = L"") { return Param(m_handle, name).GetString(defaultValue); }

    bool HasNumber(const wchar_t* name) { return Param(m_handle, name).HasNumber(); }
    void SetNumber(const wchar_t* name, int32_t value) { Param(m_handle, name).SetNumber(value); }
    int32_t GetNumber(const wchar_t* name, int32_t defaultValue = 0) { return Param(m_handle, name).GetNumber(defaultValue); }

    bool HasBool(const wchar_t* name) { return Param(m_handle, name).HasBool(); }
    void SetBool(const wchar_t* name, bool value) { Param(m_handle, name).SetBool(value); }
    bool GetBool(const wchar_t* name, bool defaultValue = false) { return Param(m_handle, name).GetBool(defaultValue); }


    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... delete the next 5 lines of code:
    //
    ParameterCollection() { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    ParameterCollection(ParameterCollection&&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    ParameterCollection(const ParameterCollection&) { SPX_THROW_HR(SPXERR_NOT_IMPL); }
    ParameterCollection& operator=(ParameterCollection&&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }
    const ParameterCollection& operator=(const ParameterCollection&) { SPX_REPORT_ON_FAIL(SPXERR_NOT_IMPL); throw SPXERR_NOT_IMPL; }


private:

    // TODO: Fix SWIG such that we don't need to expose the default methods below... 
    //       And then... once fixed ... Uncomment the next 5 lines of code:
    //    
    // ParameterCollection() = delete;
    // ParameterCollection(ParameterCollection&&) = delete;
    // ParameterCollection(const ParameterCollection&) = delete;
    // ParameterCollection& operator=(ParameterCollection&&) = delete;
    // const ParameterCollection& operator=(const ParameterCollection&) = delete;

    Handle m_handle;
};


} // CARBON_NAMESPACE_ROOT
