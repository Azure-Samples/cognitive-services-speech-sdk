//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_value.h: Public API declarations for Value C++ class
//

#pragma once
#include <functional>
#include <speechapi_cxx_common.h>
#include <speechapi_c_common.h>
#include <spxdebug.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {


/// <summary>
/// Base class for values returned by the parameter collection.
/// A value may represent a string, a number (32-bit integer), or a Boolean.
/// </summary>
class Value
{
public:
    /// <summary>
    /// Construct a value from a raw value pointer.
    /// </summary>
    /// <param name="ptr">Value pointer.</param>
    explicit Value(Value* ptr = nullptr) 
        : m_delegatePtr(ptr)
    {
    }

    /// <summary>
    /// Move constructor.
    /// </summary>
    /// <param name="other">Value to construct from.</param>
    Value(Value&& other) 
        : m_delegatePtr(std::move(other.m_delegatePtr)) 
    {
        other.m_delegatePtr = nullptr;
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    virtual ~Value()
    {
        if (m_delegatePtr != nullptr)
        {
            delete m_delegatePtr;
        }
    }

    /// <summary>
    /// Checks if the value represents a string.
    /// </summary>
    /// <returns>true if value represents a string</returns>
    virtual bool IsString() { return m_delegatePtr->IsString(); }

    /// <summary>
    /// If the value represents a string, sets its value.
    /// </summary>
    /// <param name="value">Value to set</param>
    virtual void SetString(const std::wstring& value) { m_delegatePtr->SetString(value); }

    /// <summary>
    /// If the value represents a string, gets its value.
    /// Otherwise, return the specified default value, <paramref="defaultValue"/>.
    /// </summary>
    /// <param name="defaultValue">Value to return if value type is not string (defaults to empty string).</param>
    /// <returns>string value or default</returns>
    virtual std::wstring GetString(const std::wstring& defaultValue = L"") { return m_delegatePtr->GetString(defaultValue); }

    /// <summary>
    /// Checks if the value represents a number (32-bit integer).
    /// </summary>
    /// <returns>true if value represents a number</returns>
    virtual bool IsNumber() { return m_delegatePtr->IsNumber(); }

    /// <summary>
    /// If the value represents a number (32-bit integer), sets its value.
    /// </summary>
    /// <param name="value">Value to set</param>
    virtual void SetNumber(int32_t value) { m_delegatePtr->SetNumber(value); }

    /// <summary>
    /// If the value represents a number (32-bit integer), gets its value.
    /// Otherwise, return the specified default value, <paramref="defaultValue"/>.
    /// </summary>
    /// <param name="defaultValue">Value to return if value type is not number (defaults to zero).</param>
    /// <returns>number value or default</returns>
    virtual int32_t GetNumber(int32_t defaultValue = 0) { return m_delegatePtr->GetNumber(defaultValue); }

    /// <summary>
    /// Checks if the value represents a Boolean.
    /// </summary>
    /// <returns>true if value represents a Boolean</returns>
    virtual bool IsBool() { return m_delegatePtr->IsBool(); }

    /// <summary>
    /// If the value represents a Boolean, sets its value.
    /// </summary>
    /// <param name="value">Value to set</param>
    virtual void SetBool(bool value) { m_delegatePtr->SetBool(value); }

    /// <summary>
    /// If the value represents a Boolean, gets its value.
    /// Otherwise, return the specified default value, <paramref="defaultValue"/>.
    /// </summary>
    /// <param name="defaultValue">Value to return if value type is not Boolean (defaults to false).</param>
    /// <returns>Boolean value or default</returns>
    virtual bool GetBool(bool defaultValue = false) { return m_delegatePtr->GetBool(defaultValue); }

    /// <summary>
    /// Assignment operator for a value that represents a string.
    /// </summary>
    /// <param name="value">Value to set</param>
    /// <returns>the value that was set</returns>
    const wchar_t* operator=(const wchar_t* value) { SetString(value); return value; }

    /// <summary>
    /// Assignment operator for a value that represents a string.
    /// </summary>
    /// <param name="value">Value to set</param>
    /// <returns>the value that was set</returns>
    const std::wstring& operator=(const std::wstring& value) { SetString(value); return value; }

    /// <summary>
    /// Assignment operator for a value that represents a number (32-bit integer).
    /// </summary>
    /// <param name="value">Value to set</param>
    /// <returns>the value that was set</returns>
    int32_t operator=(int32_t value) { SetNumber(value); return value; }

    /// <summary>
    /// Assignment operator for a value that represents a Boolean.
    /// </summary>
    /// <param name="value">Value to set</param>
    /// <returns>the value that was set</returns>
    bool operator=(bool value) { SetBool(value); return value; }

    /// <summary>
    /// Conversion operator for value that represent a string.
    /// <summary>
    /// <returns>string value or empty string</returns>
    operator const std::wstring() { return GetString(); }

    /// <summary>
    /// Conversion operator for value that represent a number (32-bit integer).
    /// <summary>
    /// <returns>number value or 0</returns>
    operator int32_t() { return GetNumber(); }

    /// <summary>
    /// Conversion operator for value that represent a Boolean
    /// <summary>
    /// <returns>Boolean value or false</returns>
    operator bool() { return GetBool(); }

private:

    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;
    Value& operator=(Value&&) = delete;

    Value* m_delegatePtr;
};


class BaseValueCollection
{
public:

    virtual ~BaseValueCollection() = default;

    virtual Value operator[](const std::wstring& name) = 0;

    virtual bool ContainsString(const std::wstring& name) = 0;
    virtual void SetString(const std::wstring& name, const std::wstring& value) = 0;
    virtual std::wstring GetString(const std::wstring& name, const std::wstring& defaultValue = L"") = 0;

    virtual bool ContainsNumber(const std::wstring& name) = 0;
    virtual void SetNumber(const std::wstring& name, int32_t value) = 0;
    virtual int32_t GetNumber(const std::wstring& name, int32_t defaultValue = 0) = 0;

    virtual bool ContainsBool(const std::wstring& name) = 0;
    virtual void SetBool(const std::wstring& name, bool value) = 0;
    virtual bool GetBool(const std::wstring& name, bool defaultValue = false) = 0;

protected:

    /*! \cond PROTECTED */

    BaseValueCollection() = default;

    /*! \endcond */

private:

    DISABLE_COPY_AND_MOVE(BaseValueCollection);
};


template<class Handle, class T>
class HandleValueCollection : public BaseValueCollection
{
public:
    virtual ~HandleValueCollection() = default;

    Value operator[](const std::wstring& name) override { return Value(new T(m_handle, name)); }

    bool ContainsString(const std::wstring& name) override { return T(m_handle, name).IsString(); }
    void SetString(const std::wstring& name, const std::wstring& value) override { T(m_handle, name).SetString(value); }
    std::wstring GetString(const std::wstring& name, const std::wstring& defaultValue = L"") override { return T(m_handle, name).GetString(defaultValue); }

    bool ContainsNumber(const std::wstring& name) override { return T(m_handle, name).IsNumber(); }
    void SetNumber(const std::wstring& name, int32_t value) override { T(m_handle, name).SetNumber(value); }
    int32_t GetNumber(const std::wstring& name, int32_t defaultValue = 0) override { return T(m_handle, name).GetNumber(defaultValue); }

    bool ContainsBool(const std::wstring& name) override { return T(m_handle, name).IsBool(); }
    void SetBool(const std::wstring& name, bool value) override { T(m_handle, name).SetBool(value); }
    bool GetBool(const std::wstring& name, bool defaultValue = false) override { return T(m_handle, name).GetBool(defaultValue); }

protected:

    /*! \cond PROTECTED */

    explicit HandleValueCollection(Handle handle) : m_handle(handle) { }

    Handle m_handle;

    /*! \endcond */

private:

    DISABLE_DEFAULT_CTORS(HandleValueCollection);
};


} } } // Microsoft::CognitiveServices::Speech
