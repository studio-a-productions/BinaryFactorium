/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include "common.hpp"
#include <Fri3d.h>

#if BINF_PLATFORM == DESKTOP_SDL
#include <SDL3/SDL.h>
#endif

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
    #if BINF_PLATFORM != DESKTOP_SDL
    // SPI Interface
    SPIClass& GetSPI();
    void WaitForSPI();
    #endif

    #if BINF_PLATFORM == FRI3D2026
    extern Fri3dXP expander;
    #endif
}