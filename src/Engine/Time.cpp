/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    Time lastFrameTime = 0U;
    Time frameTime = 0U;
    Time requestTime = 0U;

    void UpdateTime() {
        const Time curTime = millis();
        frameTime = curTime - lastFrameTime;
        lastFrameTime = curTime;
        requestTime = curTime;
    }

    Time DeltaTime() {
        return frameTime;
    }

    Time ReqTime() {
        Time timedif = millis() - requestTime;
        requestTime = millis();
        return timedif;
    }

    void Wait(Time mil) {
        delay(mil);
    }

    Time GetTime() {
        return millis();
    }

}