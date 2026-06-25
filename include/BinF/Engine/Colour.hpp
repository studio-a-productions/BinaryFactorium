#pragma once

#include "common.hpp"

namespace BinF::Engine {
    using colour = u16;

    constexpr inline colour ToColour(u8 r, u8 g, u8 b) {
        return (colour)(
        ((r & 0xF8) << 8) |
        ((g & 0xFC) << 3) |
        ((b & 0xF8) >> 3)
        );
    }


    constexpr colour black  = 0x0000;
    constexpr colour blue   = 0x001F;
    constexpr colour green  = 0x07E0;
    constexpr colour red    = 0xF800;
    constexpr colour white  = u16m;

}