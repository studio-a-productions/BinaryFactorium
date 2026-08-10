/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once

#include "World.hpp"

namespace BinF::Game {
    using Seed = BinF::u32;
    
    // set seed 
    void SetSeed(Seed);

    // generate chunk
    ChunkData& GenerateChunk(WorldAxis, WorldAxis);
}