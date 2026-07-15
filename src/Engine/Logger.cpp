#include <BinF/Engine/Logger.hpp>

namespace BinF::Engine {
    bool BeenInit = false;

    LoggerClass::LoggerClass(const LogLevel lvl) : m_lvl{lvl} {
        if (!BeenInit) {
            Serial.begin(115200);
            BeenInit = true;
        }

    }

}