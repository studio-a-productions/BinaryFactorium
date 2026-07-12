/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/
#pragma once

#include "common.hpp"

namespace BinF::Engine {
    using LogMessage = const char*;
    enum class LogLevel : u8 {
        Critical = 0,
        Error,
        Warning,
        Info
    };


    class LoggerClass {
    public:
        LoggerClass(LogLevel lvl = LogLevel::Error);
        ~LoggerClass() = default;

        template<typename... Args>
        inline void Log(LogMessage fmt, LogLevel lvl, Args&&... args) {
            LogMessage prefix = "";
            
            switch (lvl) {
            case LogLevel::Info:        prefix = "[INFO]"; break;
            case LogLevel::Warning:     prefix = "[WARN]"; break;
            case LogLevel::Error:       prefix = "[ERROR]"; break;
            case LogLevel::Critical:    prefix = "[CRIT]"; break;
            }

            if (lvl <= m_lvl) {
                Serial.printf("%s ", prefix);
                Serial.printf(fmt, std::forward<Args>(args)...);
            }
        }

        template<typename... Args> void Crit(LogMessage fmt, Args&&... args)     { Log(fmt, LogLevel::Critical, std::forward<Args>(args)...); }
        template<typename... Args> void Error(LogMessage fmt, Args&&... args)    { Log(fmt, LogLevel::Error, std::forward<Args>(args)...); }
        template<typename... Args> void Warn(LogMessage fmt, Args&&... args)     { Log(fmt, LogLevel::Warning, std::forward<Args>(args)...); }
        template<typename... Args> void Info(LogMessage fmt, Args&&... args)     { Log(fmt, LogLevel::Info, std::forward<Args>(args)...); }
    private:
        const LogLevel m_lvl;
    };
}

namespace BinF {
    extern Engine::LoggerClass Logger;
}