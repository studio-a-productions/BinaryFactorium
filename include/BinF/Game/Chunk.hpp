#pragma once

#include <BinF/Engine.hpp>
#include "Tile.hpp"

namespace BinF::Game {
    struct ChunkTiles {
        Tile data[256];
    };
}