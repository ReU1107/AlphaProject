#pragma once
#include "Logger/Logger.h"

namespace Alpha
{
    void BreakDebug();
}

// 報告
#define ALPHA_LOG_INFO(fmt, ...) \
    ([&](const char* file, int line) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Info, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
        } \
    )(__FILE__, __LINE__)

// 警告
#define ALPHA_LOG_WARN(fmt, ...) \
    ([&](const char* file, int line) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Warning, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
        } \
    )(__FILE__, __LINE__)

// エラー
#define ALPHA_LOG_ERROR(fmt, ...) \
    ([&](const char* file, int line) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Error, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
            Alpha::BreakDebug(); \
        } \
    )(__FILE__, __LINE__)

#define ALPHA_CHECK_ERROR(cond, fmt, ...) \
    ([&](bool condition, const char* file, int line) \
        { \
        if (!condition) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Error, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
            Alpha::BreakDebug(); \
            return true; \
        } \
        return false; \
    })(cond, __FILE__, __LINE__)

// 強制終了
#define ALPHA_CHECK_FATAL(cond, fmt, ...) \
    ([&](bool condition, const char* file, int line) \
        { \
        if (!condition) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Fatal, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
            Alpha::BreakDebug(); \
            return true; \
        } \
        return false; \
    })(cond, __FILE__, __LINE__)