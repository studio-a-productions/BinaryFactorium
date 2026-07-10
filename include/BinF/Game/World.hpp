/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include <BinF/Types.hpp>
#include <BinF/Engine.hpp>
#include <BinF/Game/Chunk.hpp>

namespace BinF::Game {
    // Chunk Coordinates
    using WorldAxis = s32;

    // moves the world based on Camera
    void UpdateWorld();
    
    // renders locally stored buffer
    void RenderWorld();

}
