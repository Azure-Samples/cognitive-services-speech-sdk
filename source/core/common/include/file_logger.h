//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// file_logger.h: FileLogger() implementation declaration
//

#include <string>

class FileLogger
{
public:
    static FileLogger& Instance();

    FileLogger(FileLogger const&) = delete;
    void operator=(FileLogger const&) = delete;

    void SetFilename(std::string&& name);
    std::string GetFilename();
    bool IsFileLoggingEnabled();
    void CloseFile();

    void LogToFile(std::string&& format, va_list argptr);
private:
    FileLogger() {};

    std::string filename;
    FILE* file = nullptr;
    std::mutex mtx;
};
