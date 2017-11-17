// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*PUWP_NAME_TO_PATH)(const char* name, wchar_t* out_path, size_t out_path_size_in_words);

extern void set_uwp_name_to_path(PUWP_NAME_TO_PATH pfunc);

#ifdef __cplusplus
}
#endif /* __cplusplus */
