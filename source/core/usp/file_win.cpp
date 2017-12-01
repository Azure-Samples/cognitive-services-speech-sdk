// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <string>
#include "file_win.h"

using namespace std;

static PUWP_NAME_TO_PATH gUwpNameToPath = NULL;

void set_uwp_name_to_path(PUWP_NAME_TO_PATH pfunc)
{
	gUwpNameToPath = pfunc;
}

static void generate_path(wchar_t* path, int size, const char* name)
{
    memset(path, 0, size);

    if (gUwpNameToPath)
    {
        gUwpNameToPath(name, path, size / sizeof(wchar_t));
    }
    else
    {
        string sName(name);
        wstring wsPath{ sName.begin(), sName.end() };
        wcsncpy(path, wsPath.data(), wsPath.length());
    }
}

extern "C" void* file_open(const char* name, const char* mode)
{
    wchar_t path[512] = { 0 };

    generate_path(path, sizeof(path), name);

	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwCreationDisposition = OPEN_EXISTING;
	string sMode(mode);
	if (string::npos != sMode.find('w'))
	{
		dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = CREATE_ALWAYS;
	}
	else if (string::npos != sMode.find('r'))
	{
		dwDesiredAccess = GENERIC_READ;
		dwCreationDisposition = OPEN_EXISTING;
	}

	void* hFile = 0;
	hFile = CreateFile2(path, dwDesiredAccess, FILE_SHARE_READ, dwCreationDisposition, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return 0;
	}
	return hFile;
}

extern "C" int file_write(void* hFile, const void* data, size_t size)
{
	DWORD dwBytesWritten = 0;
	if (FALSE == WriteFile(hFile, data, (DWORD)size, &dwBytesWritten, NULL))
	{
		return -1;
	}
	return dwBytesWritten;
}

extern "C" int file_read(void* hFile, void* data, size_t size)
{
	DWORD dwBytesRead = 0;

	if (FALSE == ReadFile(hFile, data, (DWORD)size, &dwBytesRead, NULL))
	{
		return -1;
	}
	return dwBytesRead;
}

extern "C" int file_size(void* handle)
{
	LARGE_INTEGER ret;
	if (0 != GetFileSizeEx(handle, &ret))
	{
		return ret.LowPart;
	}
	return -1;
}

extern "C" void file_close(void* hFile)
{
	CloseHandle(hFile);
}

extern "C" int file_remove(const char* name)
{
    wchar_t path[512] = { 0 };

    generate_path(path, sizeof(path), name);

    if(!DeleteFileW(path))
    {
        return -1;
    }

    return 0;
}
