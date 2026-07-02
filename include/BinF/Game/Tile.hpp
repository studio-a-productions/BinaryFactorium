/*
    Copyright (c) 2026 BinF Developers
    Licensed under the Apache License, Version 2.0
*/


#pragma once

#include <BinF/Types.hpp>

namespace BinF::Game {

    /* 
        x (is buildable) x (no walk) xxxxxx (sprite index)
    */
    constexpr u8 TileSize = 16;
    using Tile = u8;
    using TileSprite = Engine::colourID[TileSize*TileSize];


    // access helpers, could be macros, but for type safety they're just inline functions

    inline bool TileBuildable(Tile tile) {
        return 1 & (tile >> 7);
    }

    inline bool TileWalkable(Tile tile) {
        return 1 & (tile >> 6);
    }

    inline u8 TileSpriteIndx(Tile tile) {
        return tile & 63;
    }
    
    // this is actually a 2D array [64][256]
    const TileSprite TileSprites[64] = {  };


}