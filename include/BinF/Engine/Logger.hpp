/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/
#pragma once

#include "common.hpp"
#if BINF_PLATFORM == DESKTOP_SDL
#include <SDL3/SDL_log.h>
#endif

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
        void Log(LogMessage fmt, LogLevel lvl, Args&&... args) {
            LogMessage prefix = "";

            switch (lvl) {
                case LogLevel::Info:        prefix = "[INFO]"; break;
                case LogLevel::Warning:     prefix = "[WARN]"; break;
                case LogLevel::Error:       prefix = "[ERROR]"; break;
                case LogLevel::Critical:    prefix = "[CRIT]"; break;
            }

            if (lvl <= m_lvl) {
            #if BINF_PLATFORM != DESKTOP_SDL
                Serial.printf("%s ", prefix);
                Serial.printf(fmt, std::forward<Args>(args)...);
                Serial.printf("\n");
            #else 
                char fullFmt[1024];
                snprintf(fullFmt, sizeof(fullFmt), "%s %s", prefix, fmt);
                SDL_Log(fullFmt, std::forward<Args>(args)...);
            #endif
            }
        }
        
        template<typename... Args> void Crit(LogMessage fmt, Args&&... args)     { 
            #if BINF_LOGLEVEL >= BINF_CRIT  
            Log(fmt, LogLevel::Critical, std::forward<Args>(args)...); 
            #endif
        }
        template<typename... Args> void Error(LogMessage fmt, Args&&... args)    { 
            #if BINF_LOGLEVEL >= BINF_ERROR
            Log(fmt, LogLevel::Error, std::forward<Args>(args)...); 
            #endif
        }
        template<typename... Args> void Warn(LogMessage fmt, Args&&... args)     { 
            #if BINF_LOGLEVEL >= BINF_WARN
            Log(fmt, LogLevel::Warning, std::forward<Args>(args)...); 
            #endif
        }
        template<typename... Args> void Info(LogMessage fmt, Args&&... args)     { 
            #if BINF_LOGLEVEL >= BINF_INFO
            Log(fmt, LogLevel::Info, std::forward<Args>(args)...); 
            #endif
        }
    private:

        const LogLevel m_lvl;
    };
}

namespace BinF {
    extern Engine::LoggerClass Logger;
}