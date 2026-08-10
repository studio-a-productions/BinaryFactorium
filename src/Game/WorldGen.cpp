/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/WorldGen.hpp>

namespace BinF::Game {
    /*
        BinF will use a three-pass system (for now), each working on the last.

        Pass 1: generate terrain (dirt, stone, grass)
        Pass 2: generate ore veins (stone-only)
        Pass 3: decorations
    */

    // settings
    // controls density of terrain
    constexpr f32 TerrainFreq = 0.42f;
}