/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#pragma once
#include <BinF/Types.hpp>

namespace BinF::Game {
    using WorldAxis = s64;

    // moves the world based on input 
    // this handles the actual chunk data, as chunks are part of the World.
    void UpdateWorld(WorldAxis x, WorldAxis y);
    
    // renders locally stored buffer
    void RenderWorld();

}
