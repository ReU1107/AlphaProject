#pragma once
#include "Log/Logger.h"

namespace Alpha
{
    void BreakDebug();
}

// 報告
#define ALPHA_LOG_INFO(msg)    Alpha::Logger::Dispatch(Alpha::ELogLevel::Info,    __FILE__, __LINE__, msg)
#define ALPHA_LOG_WARN(msg)    Alpha::Logger::Dispatch(Alpha::ELogLevel::Warning, __FILE__, __LINE__, msg)

#define ALPHA_LOG_ERROR(fmt, ...) \
    ([&](const char* file, int line) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Error, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
            Alpha::BreakDebug(); \
        } \
    )(__FILE__, __LINE__)

#define ALPHA_CHECK_ERROR(cond, msg) \
    ([&](bool condition, const char* file, int line, Alpha::StringView _msg) \
        { \
        if (condition) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Error, file, line, _msg); \
            Alpha::BreakDebug(); \
            return true; \
        } \
        return false; \
    })(cond, __FILE__, __LINE__, msg)


#define ALPHA_CHECK_FATAL(cond, fmt, ...) \
    ([&](bool condition, const char* file, int line) \
        { \
        if (condition) \
        { \
            Alpha::Logger::Dispatch(Alpha::ELogLevel::Fatal, file, line, Alpha::Format(fmt, ##__VA_ARGS__)); \
            Alpha::BreakDebug(); \
            return true; \
        } \
        return false; \
    })(cond, __FILE__, __LINE__)