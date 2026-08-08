/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

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
        return millis() - frameTime;
    }

    Time ReqTime() {
        Time timedif = millis() - requestTime;
        requestTime = millis();
        return timedif;
    }

    void Wait(Time mil) {
        delay(mil);
    }

}