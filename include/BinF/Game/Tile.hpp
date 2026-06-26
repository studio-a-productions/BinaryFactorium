#pragma once

#include <BinF/Engine.hpp>

namespace BinF::Game {

    /* 
        x (is buildable) x (no walk) xxxxxx (sprite index)
    */
    constexpr u8 TileSize = 16;
    using Tile = u8;
    using TileSprite = const Engine::colourID[TileSize*TileSize];

    inline u8 TileBuildable(Tile tile) {
        return (tile >> 7);
    }
    
    TileSprite TileSprites[64] = {  };


}