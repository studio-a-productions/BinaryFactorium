#pragma once

#include "Palette.hpp"

namespace BinF::Engine {
    struct SpriteData {
        const colourID* pxlData;
        const u8 width;
        const u8 height;
    };

    struct SpritePos {
        const screen_pos x;
        const screen_pos y;
    };
}