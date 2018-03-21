//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_helpers.h: Implementation declarations for *Http* helper methods
//

#pragma once
#include <map>
#include <string>
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


std::string SpxHttpDownloadString(const char* httpHostName, const char* httpRelativePath, unsigned int* pstatusCode = nullptr, std::map<std::string, std::string>* presponseHeaders = nullptr);


} // CARBON_IMPL_NAMESPACE
