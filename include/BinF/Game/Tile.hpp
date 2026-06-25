#pragma once

#include <BinF/Engine.hpp>

namespace BinF::Game {

    /* 
        Tiles in BinF are 8 bit ids, with 2 first bits being direction and the rest the actual type, which leaves 64 types.
        00: north
        01: east
        10: south
        11: west

        [ xx (direction) xxxxxx (type)]
    */
    using Tile = u8;
}