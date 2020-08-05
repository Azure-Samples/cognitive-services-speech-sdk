//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>

#include <interfaces/base.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxKwsModel : public ISpxInterfaceBaseFor<ISpxKwsModel>
{
public:
    virtual void InitFromFile(const wchar_t* fileName) = 0;
    virtual std::wstring GetFileName() const = 0;
};

} } } }