#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    Time frameTime = 0;
    Time requestTime = 0;

    void UpdateTime() {
        frameTime = millis();
        requestTime = millis();
    }

    Time DeltaTime() {
        return frameTime - millis();
    }

    Time ReqTime() {
        static Time timedif = requestTime - millis();
        requestTime = millis();
        return timedif;
    }

}