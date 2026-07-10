/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/


#pragma once

#include "Tile.hpp"

namespace BinF::Game {
    // Sub-Chunk Coordinates
    // (0, 0) is upper-left of chunk
    using PixelOffset = Engine::screen_pos;
    
    constexpr u16 ChunkTiles = 16U*16U;
    constexpr u16 ChunkSize  = 16U;
    constexpr PixelOffset ChunkPixels = ChunkSize*TileSize;

    struct ChunkData {
        Tile tiles[ChunkTiles]; // ChunkTiles array 
        // other data may later be added
    };
}