// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license informatio
#include <stdio.h>
#include <stdlib.h>
#include "azure_c_shared_utility/crt_abstractions.h"

static void generate_path(char* path, int size, const char* name)
{
    strcpy_s(path, size, getenv("HOME"));
    int n = strlen(path);

    if (n > 0 && path[n - 1] != '/')
    {
        strcat_s(path, size, "/");
    }

    strcat_s(path, size, name);
}

void* file_open(const char* name, const char* mode)
{
    char path[1024];

    generate_path(path, sizeof(path), name);

    return fopen(path, mode);
}

int file_write(void* hFile, const void* data, size_t size)
{
	return fwrite(data, 1, size, hFile);
}

int file_read(void* hFile, void* data, size_t size)
{
	return fread(data, 1, (size_t)size, hFile);;
}

int file_size(void* handle)
{
	fseek(handle, 0, SEEK_END);
	size_t size = (size_t)ftell(handle);
	fseek(handle, 0, SEEK_SET);
	return size;
}

void file_close(void* hFile)
{
	fclose(hFile);
}

int file_remove(const char* name)
{
    char path[1024];

    generate_path(path, sizeof(path), name);
   
    return remove(path);
}
