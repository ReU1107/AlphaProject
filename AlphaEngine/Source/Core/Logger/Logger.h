#pragma once
#include "Core/String/String.h"
#include <fstream>

namespace Alpha 
{

    enum class ELogLevel 
    { 
        Info,       // 情報通知、自由に使って
        Warning,    // 警告、代替え手段でアプリ自体は続行
        Error,      // 不具合、デバッガーで止める
        Fatal,      // 続行不能、アプリケーションを終了
    };

    class Logger
    {
    private:
        std::ofstream m_file{};
        ELogLevel m_min_level = ELogLevel::Info;

    public:
        // 全てのログの入り口
        static void Dispatch(ELogLevel level, const char* file, int line, StringView message);

    private:
        static StringView GetLevelTag(ELogLevel level);
    };

}