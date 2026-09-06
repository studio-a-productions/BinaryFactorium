#include <BinF/Engine/Logger.hpp>

#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    bool BeenInit = false;

    LoggerClass::LoggerClass(const LogLevel lvl) : m_lvl{lvl} {
        if (!BeenInit) {
            #if BINF_PLATFORM != DESKTOP_SDL
            Serial.begin(115200);
            #else
            SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, "");
            #endif
            BeenInit = true;
        }

    }
    template<typename... Args>
    void LoggerClass::Log(LogMessage fmt, LogLevel lvl, Args&&... args) {
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

}