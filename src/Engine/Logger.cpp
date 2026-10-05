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
}