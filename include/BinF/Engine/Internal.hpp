/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include "common.hpp"
namespace BinF::Engine {
    // Input
    void InitInput();
    void UpdateInput();
    void ExitInput(); // idk man

    // Time
    void UpdateTime();

    // Renderer
    void InitRenderer();
    void ExitRenderer();

    // SPI Interface
    SPIClass& GetSPI();
}