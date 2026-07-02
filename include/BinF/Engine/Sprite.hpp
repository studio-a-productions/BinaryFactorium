/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "Palette.hpp"

namespace BinF::Engine {
    struct SpriteData {
        const colourID* pxlData;
        const u8 width;
        const u8 height;
    };


    // 2D Position data
    // BinF::Engine makes "origin" the left-upper-most pixel
    struct SpritePos {
        const screen_pos x;
        const screen_pos y;
    };
}