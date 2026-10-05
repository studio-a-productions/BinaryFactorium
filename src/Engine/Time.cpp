/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    #if BINF_PLATFORM == DESKTOP_SDL
    static inline Time millis() {
        return SDL_GetTicks();
    }
    #endif

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
        #if BINF_PLATFORM == FRI3D2026 || BINF_PLATFORM == FRI3D2024
        delay(mil);
        #elif BINF_PLATFORM == DESKTOP_SDL
        SDL_Delay(mil);
        #endif
    }

    Time GetTime() {
        return millis();
    }

}