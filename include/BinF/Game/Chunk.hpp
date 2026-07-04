/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/


#pragma once

#include "Tile.hpp"

namespace BinF::Game {
    using WorldChunkPos = s32;
    using ChunkID = u64; // id used to retrieve from SD storage
    
    constexpr u16 ChunkTiles = 16*16;
    
    // generates a chunk using the chunk algorithm
    ChunkData& GenerateChunk(WorldChunkPos x, WorldChunkPos y);

    // captures chunk from storages
    ChunkData& LoadChunk(u64 id);

    struct ChunkData {
        Tile* tiles; // ChunkTiles array
        // other data may later be added
    };
}