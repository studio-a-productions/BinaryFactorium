/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/

#include <BinF/Game/World.hpp>
#include <BinF/Game/Chunk.hpp>


namespace BinF::Game {
    // the amount of chunks
    constexpr u8 LocalChunksAmount = 9; /* 3x3*/


    // data COULD be stored in PSRAM, but preferably not (PSRAM = slow ;-;)
    ChunkData* WorldView[LocalChunksAmount] = { nullptr };
    
    // function that handles updating world data and rendering them.
    // the actual code underneath this isn't an issue at the moment.
    // 
    // potentionally, when a fast-travel system is implemented, this might become a "task" (loading stuff)
    // so that there can be some kind of animation to mask up the loading. 
    // (again, future thinking, not our priority)
    void UpdateWorld(WorldAxis x, WorldAxis y) {
        // NOT PRIORITY
        // IGNORE FUNCTION FOR NOW
    }

    void RenderWorld() {
        // render visible tiles from WorldView (basically 20x20 tiles you'll need to scan (max) from the camera's position (center of screen))
        // make sure that you're clipping the edges off using 
        // use DrawSpriteStride () *see BinF/Engine.hpp
    }


}