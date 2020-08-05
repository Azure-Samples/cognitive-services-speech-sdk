//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <string>

#include <interfaces/base.h>
#include <interfaces/results.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxSessionEventArgs : public ISpxInterfaceBaseFor<ISpxSessionEventArgs>
{
public:
    virtual const std::wstring& GetSessionId() = 0;
};

class ISpxSessionEventArgsInit : public ISpxInterfaceBaseFor<ISpxSessionEventArgsInit>
{
public:
    virtual void Init(const std::wstring& sessionId) = 0;
};

class ISpxRecognitionEventArgs :
    public virtual ISpxSessionEventArgs,
    public ISpxInterfaceBaseFor<ISpxRecognitionEventArgs>
{
public:
    virtual const uint64_t& GetOffset() = 0;
    virtual std::shared_ptr<ISpxRecognitionResult> GetResult() = 0;
};

class ISpxRecognitionEventArgsInit : public ISpxInterfaceBaseFor<ISpxRecognitionEventArgsInit>
{
public:
    virtual void Init(const std::wstring& sessionId, std::shared_ptr<ISpxRecognitionResult> result) = 0;
    virtual void Init(const std::wstring& sessionId, uint64_t offset) = 0;
};

class ISpxConnectionEventArgs :
    public virtual ISpxSessionEventArgs,
    public ISpxInterfaceBaseFor<ISpxConnectionEventArgs>
{
};

class ISpxConnectionEventArgsInit : public ISpxInterfaceBaseFor<ISpxConnectionEventArgsInit>
{
public:
    virtual void Init(const std::wstring& sessionId) = 0;
};


/// <summary>
/// Interface for miscellaneous conversation related event arguments (e.g. expiration, participants
/// changed, etc...)
/// </summary>
class ISpxConversationEventArgs :
    public virtual ISpxSessionEventArgs,
    public virtual ISpxInterfaceBaseFor<ISpxConversationEventArgs>
{
};


} } } }