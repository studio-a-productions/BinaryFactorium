/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Engine.hpp>
#include <SPI.h>
#include <BinF/Engine/Internal.hpp>

namespace BinF::Engine {
    void Init() {
        Serial.begin(115200);
        Serial.println("[BinF::Engine] Init SPI");
        SPI.begin(7, 8, 6);
        Serial.println("[BinF::Engine] InitInput()");
        InitInput();
        Serial.println("[BinF::Engine] InitRenderer()");
        InitRenderer();

        FileSystemClass FileSystem = FileSystemClass();
        FileSystem.Begin();
    }

    void Update() {
        UpdateTime();
        UpdateInput();
    }

    void Exit() {
        // while (true) scream("coffee!")
        ExitRenderer();
    }
}