//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_keyword_recognition_model.h: Public API declarations for KeywordRecognitionModel related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) KeywordRecognitionModel_Handle_IsValid(SPXKEYWORDHANDLE hkeyword);
SPXAPI KeywordRecognitionModel_Handle_Close(SPXKEYWORDHANDLE hkeyword);

SPXAPI KeywordRecognitionModel_Create_From_File(const wchar_t* fileName, SPXKEYWORDHANDLE* phkwmodel);
