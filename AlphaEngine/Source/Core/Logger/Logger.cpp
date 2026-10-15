#include "Logger.h"
#include <Windows.h>
#include <iostream>

namespace Alpha
{
    void Logger::Dispatch(ELogLevel level, const char* file, int line, StringView message)
    {
        // ログ成形
        String output = Text("[") + GetLevelTag(level) + Text("] ") +
            Text(file) + Text("(") + Text(std::to_string(line)) + Text("): ") +
            String(message) + Text("\n");

        OutputDebugStringW(output.c_str());

        std::wcerr << output.c_str();
    }

    StringView Logger::GetLevelTag(ELogLevel level)
    {
        switch (level)
        {
            case ELogLevel::Info:    return L"INFO";
            case ELogLevel::Warning: return L"WARN";
            case ELogLevel::Error:   return L"ERR ";
            case ELogLevel::Fatal:   return L"FATAL";
        }
        return L"UNKNOWN";
    }
}